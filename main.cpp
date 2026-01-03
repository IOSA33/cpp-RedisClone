#include <iostream>
#include "NetWork/Server.h"

// g++ main.cpp Redis.cpp Logger.cpp NetWork/Server.cpp -lws2_32 -o app
int main() {
    // TODO: Multiple clients connections
    std::cout << "Glad to see you!\n";    
    
    // Redis redis{"log.txt", "Snapshot.txt"};
    // redis.run();

    TCPServer server{ 8080 };
    server.run();
}