#include <iostream>
#include "NetWork/Server.h"

// g++ main.cpp Redis.cpp Logger.cpp NetWork/Server.cpp -lws2_32 -o app
int main() {
    std::cout << "Glad to see you!\n";    
    TCPServer server{ 8080 };
    server.run();
}