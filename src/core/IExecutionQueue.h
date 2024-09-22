#pragma once

#include <future>
#include <memory>

namespace core {
template <typename Unused> class IExecutionQueue;

/// @brief High-level interface that provides access to queue-based tasks execution.
/// @tparam T Type of the object to be processed on the queue.
/// @tparam R Type of the result of object processing. Can be 'void'.
template <typename T, typename R> class IExecutionQueue<R(T)>
{
public:
    virtual ~IExecutionQueue() = default;

    /// @brief Pushes-by-copy an object to be processed on the queue.
    /// @return Future object to obtain result when the task is done.
    std::future<R> push(const T & object);

    /// @brief Pushes-by-move an object to be processed on the queue.
    /// @return Future object to obtain result when the task is done.
    std::future<R> push(T && object);

    /// @brief Emplaces an object to be processed on the queue.
    /// @return Future object to obtain result when the task is done.
    template <typename... Args> std::future<R> emplace(Args &&... args);

    /// @brief Makrs all tasks as canceled.
    virtual void cancel() = 0;

private:
    virtual std::future<R> pushImpl(std::unique_ptr<T> object) = 0;
};
} // namespace core

template <typename T, typename R> std::future<R> core::IExecutionQueue<R(T)>::push(const T & object)
{
    return pushImpl(std::unique_ptr<T>(new T{object}));
}

template <typename T, typename R> std::future<R> core::IExecutionQueue<R(T)>::push(T && object)
{
    return pushImpl(std::unique_ptr<T>(new T{std::move(object)}));
}

template <typename T, typename R>
template <typename... Args>
std::future<R> core::IExecutionQueue<R(T)>::emplace(Args &&... args)
{
    return pushImpl(std::unique_ptr<T>(new T{std::forward<Args>(args)...}));
}
