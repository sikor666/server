#pragma once

#include "CancelTokenProvider.h"
#include "ExecutionPool.h"
#include "IExecutionQueue.h"

#include <queue>

namespace core {
template <typename R, typename T> struct QueuedObject
{
    std::unique_ptr<T> object;
    std::promise<R> promise;
    CancelToken cancelToken;
};

template <typename R, typename T> class ExecutionQueue : public IExecutionQueue<R(T)>, private ITaskProvider
{
public:
    ExecutionQueue(std::shared_ptr<IExecutionPool> executionPool, const IThreadWorkerFactory & workerFactory,
        std::function<R(const std::atomic_bool & isCanceled, T && object)> executor);
    ~ExecutionQueue();

public: // IExecutionQueue
    virtual void cancel() final;

private: // IExecutionQueue
    virtual std::future<R> pushImpl(std::unique_ptr<T> object) final;

private: // IThreadWorkerPoolTaskProvider
    virtual Task nextTask() final;

private:
    void execute(T && object, std::promise<void> & promise, const std::atomic_bool & canceled);
    template <typename Y> void execute(T && object, std::promise<Y> & promise, const std::atomic_bool & canceled);

    void pushObject(std::unique_ptr<QueuedObject<R, T>> object);
    std::unique_ptr<QueuedObject<R, T>> popObject();

    bool hasTask();
    void waitAllTasks();

private:
    std::atomic_size_t m_taskRunningCount{0};

    std::atomic_bool m_hasTask{false};
    std::queue<std::unique_ptr<QueuedObject<R, T>>> m_taskQueue;
    std::mutex m_taskQueueMutex;
    std::condition_variable m_taskQueueCondition;

    CancelTokenProvider m_cancelTokenProvider;

    const std::shared_ptr<IExecutionPool> m_executionPool;
    const std::function<R(const std::atomic_bool & isCanceled, T && object)> m_executor;
};
} // namespace core

template <typename R, typename T>
core::ExecutionQueue<R, T>::ExecutionQueue(std::shared_ptr<IExecutionPool> executionPool,
    const IThreadWorkerFactory & workerFactory,
    std::function<R(const std::atomic_bool & shouldQuit, T && object)> executor)
    : m_executionPool(executionPool)
    , m_executor(std::move(executor))
{
    if (m_executionPool)
    {
        m_executionPool->addProvider(*this);
    }
}

template <typename R, typename T> core::ExecutionQueue<R, T>::~ExecutionQueue()
{
    m_cancelTokenProvider.cancel();
    waitAllTasks();
    if (m_executionPool)
    {
        m_executionPool->removeProvider(*this);
    }
}

// IExecutionQueue

template <typename R, typename T> std::future<R> core::ExecutionQueue<R, T>::pushImpl(std::unique_ptr<T> object)
{
    using QueuedObject = QueuedObject<R, T>;

    std::promise<R> promise;
    std::future<R> future = promise.get_future();

    std::unique_ptr<QueuedObject> queuedObject(
        new QueuedObject{std::move(object), std::move(promise), m_cancelTokenProvider.token()});

    pushObject(std::move(queuedObject));
    if (m_executionPool)
    {
        m_executionPool->notifyOneWorker();
    }

    return future;
}

template <typename R, typename T> void core::ExecutionQueue<R, T>::cancel()
{
    m_cancelTokenProvider.cancelAndRenew();
}

// IThreadWorkerPoolTaskProvider

template <typename R, typename T> core::Task core::ExecutionQueue<R, T>::nextTask()
{
    if (not hasTask())
    {
        return Task();
    }

    m_taskRunningCount++;
    return Task([&] {
        std::unique_ptr<QueuedObject<R, T>> object = popObject();
        if (object)
        {
            execute(std::move(*object->object), object->promise, *object->cancelToken);
        }

        if (--m_taskRunningCount > 0)
        {
            return;
        }

        if (not m_hasTask)
        {
            m_taskQueueCondition.notify_all();
        }
    });
}

// Private

template <typename R, typename T>
void core::ExecutionQueue<R, T>::execute(T && object, std::promise<void> & promise, const std::atomic_bool & canceled)
{
    try
    {
        m_executor(canceled, std::move(object));
        promise.set_value();
    }
    catch (...)
    {
        try
        {
            promise.set_exception(std::current_exception());
        }
        catch (...)
        {
        } // set_exception() may throw too
    }
}

template <typename R, typename T>
template <typename Y>
void core::ExecutionQueue<R, T>::execute(T && object, std::promise<Y> & promise, const std::atomic_bool & canceled)
{
    try
    {
        promise.set_value(m_executor(canceled, std::move(object)));
    }
    catch (...)
    {
        try
        {
            promise.set_exception(std::current_exception());
        }
        catch (...)
        {
        } // set_exception() may throw too
    }
}

template <typename R, typename T> void core::ExecutionQueue<R, T>::pushObject(std::unique_ptr<QueuedObject<R, T>> object)
{
    std::lock_guard<std::mutex> lock(m_taskQueueMutex);

    m_hasTask = true;
    m_taskQueue.push(std::move(object));
}

template <typename R, typename T> std::unique_ptr<core::QueuedObject<R, T>> core::ExecutionQueue<R, T>::popObject()
{
    std::lock_guard<std::mutex> lock(m_taskQueueMutex);
    if (m_taskQueue.empty())
    {
        return nullptr;
    }

    std::unique_ptr<QueuedObject<R, T>> object = std::move(m_taskQueue.front());
    m_taskQueue.pop();

    m_hasTask = not m_taskQueue.empty();

    return object;
}

template <typename R, typename T> bool core::ExecutionQueue<R, T>::hasTask()
{
    if (not m_hasTask)
    {
        return false;
    }

    return true;
}

template <typename R, typename T> void core::ExecutionQueue<R, T>::waitAllTasks()
{
    std::unique_lock<std::mutex> lock(m_taskQueueMutex);
    while (m_taskRunningCount > 0 or not m_taskQueue.empty())
    {
        m_taskQueueCondition.wait(lock);
    }
}
