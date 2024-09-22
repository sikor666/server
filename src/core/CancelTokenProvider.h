#pragma once

#include <atomic>
#include <memory>
#include <mutex>

namespace core {
using CancelToken = std::shared_ptr<std::atomic_bool>;
class CancelTokenProvider
{
public:
    CancelToken token();
    void cancel();
    void cancelAndRenew();

private:
    void cancelAndRenew(const bool renew);

private:
    CancelToken m_currentToken = std::make_shared<std::atomic_bool>(false);
    std::mutex m_mutex;
};
} // namespace core
