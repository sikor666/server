#pragma once

#include "TaskProviderList.h"

#include <atomic>
#include <memory>
#include <vector>

namespace core {
class IExecutionPool
{
public:
    virtual ~IExecutionPool() = default;

    virtual void addProvider(ITaskProvider & provider) = 0;
    virtual void removeProvider(ITaskProvider & provider) = 0;

    virtual bool notifyWorker() = 0;
};

class ExecutionPool : public IExecutionPool
{
public:
    ExecutionPool(const uint32_t threadCount, const IThreadWorkerFactory & workerFactory);

    virtual void addProvider(ITaskProvider & provider) final;
    virtual void removeProvider(ITaskProvider & provider) final;

    virtual bool notifyWorker() final;

private:
    std::atomic_bool m_valid{true};
    TaskProviderList m_providerGroup;

    std::vector<std::unique_ptr<IThreadWorker>> m_workers;
};
} // namespace core
