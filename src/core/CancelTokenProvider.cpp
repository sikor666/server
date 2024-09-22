#include "CancelTokenProvider.h"

namespace core {
CancelToken CancelTokenProvider::token()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_currentToken;
}

void CancelTokenProvider::cancel()
{
    cancelAndRenew(false);
}

void CancelTokenProvider::cancelAndRenew()
{
    cancelAndRenew(true);
}

void CancelTokenProvider::cancelAndRenew(const bool renew)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_currentToken)
    {
        *m_currentToken = true;
    }
    if (renew)
    {
        m_currentToken = std::make_shared<std::atomic_bool>(false);
    }
}
} // namespace core
