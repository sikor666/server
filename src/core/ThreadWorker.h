#pragma once

#include <atomic>
#include <condition_variable>
#include <future>
#include <mutex>
#include <thread>

namespace core {
using Task = std::packaged_task<void()>;
class ITaskProvider
{
public:
    virtual ~ITaskProvider() = default;

    virtual Task nextTask() = 0;
};

class IThreadWorker
{
public:
    virtual ~IThreadWorker() = default;

    virtual bool notifyWorker() = 0;
};

class IThreadWorkerFactory
{
public:
    static std::shared_ptr<const IThreadWorkerFactory> defaultFactory();

    virtual ~IThreadWorkerFactory() = default;

    virtual std::unique_ptr<IThreadWorker> createWorker(ITaskProvider & provider) const = 0;
};
} // namespace core
