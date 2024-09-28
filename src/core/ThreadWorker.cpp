#include "ThreadWorker.h"

namespace core {
class ThreadWorker : public IThreadWorker
{
public:
    explicit ThreadWorker(ITaskProvider & provider);
    virtual ~ThreadWorker();

    virtual bool notify() final;

private:
    void threadMain();
    void shutdown();

private:
    std::atomic_bool m_shouldQuit{false};
    std::atomic_bool m_checkNextTask{false};
    std::condition_variable m_condition;
    std::mutex m_mutex;
    std::unique_ptr<std::thread> m_thread;

    ITaskProvider & m_provider;
};
} // namespace core

std::shared_ptr<const core::IThreadWorkerFactory> core::IThreadWorkerFactory::defaultFactory()
{
    class ThreadWorkerFactory : public IThreadWorkerFactory
    {
    public:
        virtual std::unique_ptr<IThreadWorker> createWorker(ITaskProvider & provider) const final
        {
            return std::unique_ptr<IThreadWorker>(new ThreadWorker(provider));
        }
    };

    static std::shared_ptr<IThreadWorkerFactory> s_factory = std::make_shared<ThreadWorkerFactory>();
    return s_factory;
}

core::ThreadWorker::ThreadWorker(ITaskProvider & provider)
    : m_provider(provider)
{
}

core::ThreadWorker::~ThreadWorker()
{
    shutdown();
    if (m_thread and m_thread->joinable())
    {
        m_thread->join();
    }
}

bool core::ThreadWorker::notify()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_checkNextTask)
    {
        return false;
    }

    m_checkNextTask = true;
    if (not m_thread)
    {
        m_thread.reset(new std::thread(&ThreadWorker::threadMain, this));
    }

    m_condition.notify_one();

    return true;
}

void core::ThreadWorker::shutdown()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_shouldQuit = true;
    m_condition.notify_one();
}

void core::ThreadWorker::threadMain()
{
    while (true)
    {
        if (m_shouldQuit)
        {
            break;
        }

        Task task = m_provider.nextTask();

        if (task.valid())
        {
            task();
            continue;
        }

        m_checkNextTask = false;

        std::unique_lock<std::mutex> lock(m_mutex);

        if (m_shouldQuit)
        {
            break;
        }

        m_condition.wait(lock);
    }
}
