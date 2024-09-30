#pragma once

#include <cstdint>
#include <vector>

class session
{
public:
    session(int socket);
    ~session();

    void start();

private:
    int m_socket;

    std::vector<int8_t> m_data;
};
