#pragma once

#include "ThreadWorker.h"

#include <list>
#include <mutex>

namespace core {
class TaskProviderList : public ITaskProvider
{
public: // ITaskProvider
    virtual Task nextTask() final;

public:
    void addProvider(ITaskProvider & provider);
    void removeProvider(ITaskProvider & provider);

private:
    using TaskProviders_lt = std::list<ITaskProvider *>;
    TaskProviders_lt m_taskProviders;
    TaskProviders_lt::iterator m_currentTaskProviderIt;
    std::mutex m_mutex;
};
} // namespace core
