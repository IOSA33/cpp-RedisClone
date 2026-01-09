#include <iostream>
#include <WS2tcpip.h>
#include <windows.h>
#include <winsock2.h>
#include <stdio.h>
#include <vector>
#include <string>
#include "Server.h"
#include <thread>
#include <mutex>
#include "../Include/wepoll.h"

#pragma comment (lib, "ws2_32.lib");

// Declarations
int makeFakeConnection();

int TCPServer::run() {
    constexpr size_t MAX_EPOLL_EVENTS { 16 };

    WSADATA wsadata;
    int wsaerr;
    wsaerr = WSAStartup(MAKEWORD(2,2), &wsadata);
    if (wsaerr != 0) {
        std::cout << "Winsock dll not found" << std::endl;
        return 1;
    } else {
        std::cout << "winsock DLL Found" << std::endl;
        std::cout << "Status: " << wsadata.szSystemStatus << std::endl;
    }

    HANDLE epoll_fd = epoll_create1(0);

    SOCKET in = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (in == INVALID_SOCKET) {
        std::cout << "Error at socket()" << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    } else {
        std::cout << "Socket is OK" << std::endl;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(m_port);
    addr.sin_addr.S_un.S_addr = ADDR_ANY;

    if (bind(in, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        std::cout << "Can't bind socket! " << WSAGetLastError() << std::endl;
        return 1;
    }

    // Listen on port TCP
    if (listen(in, SOMAXCONN) == SOCKET_ERROR) {
        std::cout << "error in listen() : " << WSAGetLastError << std::endl;
    } else {
        std::cout << "Listen() is OK, I'm waiting for connections..." << std::endl;
    }

    // Getting input in server side, another thread
    std::thread thread_exit(&TCPServer::stop, this);

    struct epoll_event eventQueue[MAX_EPOLL_EVENTS];
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = in;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, in, &ev) == -1) {
        std::cout << "Cannot add epoll ctl!" << '\n';
        return 1;
    }


    while(getIsRunning()) {

        int nfds = epoll_wait(epoll_fd, eventQueue, MAX_EPOLL_EVENTS, -1);

        for (size_t n { 0 }; n < nfds; ++n) {
            if (eventQueue[n].data.fd == in) {
                // Accept functions
                SOCKET acceptSocket;
                acceptSocket = accept(in, NULL, NULL);
                if(acceptSocket == INVALID_SOCKET) {
                    std::cout << "accept failed:" << WSAGetLastError() << std::endl;
                    closesocket(in);
                    WSACleanup();
                    return 1;
                }
                
                u_long mode { 1 };
                ioctlsocket(acceptSocket, FIONBIO, &mode);

                ev.events = EPOLLIN;
                ev.data.fd = acceptSocket;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, acceptSocket, &ev);

                std::cout << "Client is connected!" << std::endl; 
            } else {
                // recv() Receives data from the client
                char recvBuf[1024];
                int recvBuflen = sizeof(recvBuf);
                ZeroMemory(recvBuf, 1024);

                int bytesRecv = recv(eventQueue[n].data.fd, recvBuf, recvBuflen, 0);
                if (bytesRecv > 0) {
                    // Handling request on server
                    std::string input(recvBuf, bytesRecv);

                    std::cout << "\n(User input): [" << input << "]\n";
                    std::cout << "Recieved bytes from user: [" << bytesRecv << "]\n";
                    std::string response{};
                    // If input is correct
                    if (m_redis.parser(input)) {
                        response = m_redis.executeValidCmd(Log::Logging);
                        std::cout << "(Response to the user): [" << response << "]\n";
                        m_redis.clearCurrCmd();
                    } else {
                        response = "Unknown command";
                        m_redis.clearCurrCmd();
                    }

                    // Sending back to the client response
                    int bytes_sent = send(eventQueue[n].data.fd, response.c_str(), response.size(), 0);
                    if (bytes_sent == SOCKET_ERROR) {
                        // If sending fails, print an error
                        std::cerr << "send failed: " << WSAGetLastError() << std::endl;
                    } else {
                        // Print the number of bytes sent
                        std::cout << "Sent bytes to the user: [" << bytes_sent << "] " << std::endl;
                    }

                    input.clear();
                } else {
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, eventQueue[n].data.fd, NULL);
                    closesocket(eventQueue[n].data.fd);
                    // If no data is received, print an error message
                    std::cout << "User disconnected" << '\n';
                }
            }
        }
    }

    std::cout << "Server closing!" << '\n';

    thread_exit.join();
    epoll_close(epoll_fd);
    WSACleanup();
    return 0;
}

void TCPServer::setIsRunning(bool cmd) {
    std::lock_guard<std::mutex> lk(m_mtx);
    if (cmd) {
        m_isRunning = true;
    } else {
        m_isRunning = false;
    }
}

bool TCPServer::getIsRunning() {
    std::lock_guard<std::mutex> lk(m_mtx);
    return m_isRunning;
}

void TCPServer::stop() {
    std::string input{};
    while(input != "exit") {
        std::getline(std::cin >> std::ws, input);
    }
    setIsRunning(false);
    makeFakeConnection();
}

int makeFakeConnection() {
    WSADATA wsadata;
    int wsaerr;
    wsaerr = WSAStartup(MAKEWORD(2,2), &wsadata);
    if (wsaerr != 0) {
        std::cout << "Winsock dll not found" << std::endl;
        return 1;
    }
    
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cout << "Error at socket()" << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Client side hint
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    int connResult = connect(clientSocket, (sockaddr*)&addr, sizeof(addr));
    if (connResult == SOCKET_ERROR) {
        std::cerr << "Can't connect to a server, Err: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::string input{"Closing server from fake Connection!"};
    int bytes_sent = send(clientSocket, input.c_str(), input.size(), 0);
    if (bytes_sent == SOCKET_ERROR) {
        std::cerr << "send failed: " << WSAGetLastError() << std::endl;
    }
    
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
