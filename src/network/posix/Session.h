#pragma once

#include <cstdint>
#include <vector>

class Session
{
public:
    Session(int socket);
    ~Session();

    void start();

private:
    int m_socket;

    std::vector<int8_t> m_data;
};
