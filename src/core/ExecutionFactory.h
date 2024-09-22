#pragma once

#include "ExecutionQueue.h"

#include <atomic>
#include <functional>
#include <memory>

namespace core {
/// @brief Creates pool with hardware-optimal number of threads.
std::shared_ptr<IExecutionPool> CreateExecutionPool();

/// @brief Creates concurrent queue with specific processing function.
template <typename R, typename T>
std::unique_ptr<IExecutionQueue<R(T)>> CreateConcurrentExecutionQueue(std::shared_ptr<IExecutionPool> executionPool,
    std::function<R(const std::atomic_bool & isCanceled, T && object)> executor)
{
    return std::unique_ptr<ExecutionQueue<R, T>>(
        new ExecutionQueue<R, T>(executionPool, *IThreadWorkerFactory::defaultFactory(), std::move(executor)));
}
} // namespace core
