#pragma once

class session
{
public:
    session(int socket);
    ~session();

    void start();

private:
    int m_socket;
};
