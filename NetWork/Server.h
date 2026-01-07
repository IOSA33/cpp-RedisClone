#pragma once

#include <string>
#include "../Redis.h"
#include <atomic>
#include <mutex>

class TCPServer {
private:
    std::mutex m_mtx;
    Redis m_redis{"log.txt", "Snapshot.txt"};
    int m_port{};
    bool m_isRunning{ true };

public:
    TCPServer(int port) : m_port(port) {}
    void setIsRunning(bool cmd);
    bool getIsRunning();
    int run();
    void stop();
};