#pragma once

#include <string>
#include "../Redis.h"

class TCPServer {
private:
    Redis m_redis{"Test.txt"};
    int m_port{};

public:
    TCPServer(int port) : m_port(port) {}
    int run();
};