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

bool ExecutionPool::notifyWorker()
{
    bool notified = false;
    for (const auto & worker : m_workers)
    {
        notified |= worker->notify();
        if (notified)
        {
            return true;
        }
    }

    return notified;
}
} // namespace core
