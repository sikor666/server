#include "ExecutionFactory.h"

namespace core {
uint32_t GetOptimalThreadCount()
{

    const uint32_t defaultThreadCount = 4;
    const uint32_t hardwareThreadCount = std::thread::hardware_concurrency();

    return hardwareThreadCount ? hardwareThreadCount : defaultThreadCount;
}

std::shared_ptr<IExecutionPool> CreateDefaultExecutionPool(const uint32_t threadCount)
{
    return std::make_shared<ExecutionPool>(threadCount, *IThreadWorkerFactory::defaultFactory());
}

std::shared_ptr<IExecutionPool> CreateExecutionPool()
{
    return CreateDefaultExecutionPool(GetOptimalThreadCount());
}
} // namespace core
