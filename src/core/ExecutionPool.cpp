#include "ExecutionPool.h"

namespace core {
ExecutionPool::ExecutionPool(const uint32_t threadCount, const IThreadWorkerFactory & workerFactory)
{
    for (uint32_t i = 0; i < threadCount; i++)
    {
        m_workers.emplace_back(workerFactory.createWorker(m_providerGroup));
    }
}

void ExecutionPool::addProvider(ITaskProvider & provider)
{
    m_providerGroup.addProvider(provider);
}

void ExecutionPool::removeProvider(ITaskProvider & provider)
{
    m_providerGroup.removeProvider(provider);
}

bool ExecutionPool::notifyOneWorker()
{
    return details::NotifyWorkers(m_workers, true);
}

void ExecutionPool::notifyAllWorkers()
{
    details::NotifyWorkers(m_workers, false);
}

// Details

bool details::NotifyWorkers(const std::vector<std::unique_ptr<IThreadWorker>> & workers, const bool single)
{
    bool notified = false;
    for (const auto & worker : workers)
    {
        notified |= worker->notifyWorker();
        if (notified and single)
        {
            return true;
        }
    }

    return notified;
}
} // namespace core
