#include <iostream>
#include <WS2tcpip.h>
#include <windows.h>
#include <winsock2.h>
#include <stdio.h>
#include <vector>
#include <string>
#include "Server.h"

#pragma comment (lib, "ws2_32.lib");

int TCPServer::run() {
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
    if (listen(in, 1) == SOCKET_ERROR) {
        std::cout << "error in listen() : " << WSAGetLastError << std::endl;
    } else {
        std::cout << "Listen() is OK, I'm waiting for connections..." << std::endl;
    }

    while(true) {
        // Accept functions
        SOCKET acceptSocket;
        acceptSocket = accept(in, NULL, NULL);
        if(acceptSocket == INVALID_SOCKET) {
            std::cout << "accept failed:" << WSAGetLastError() << std::endl;
            closesocket(in);
            WSACleanup();
            return 1;
        } else { 
            std::cout << "Client is connected!" << std::endl; 
        } 
        
        // Loop for communicationg between user
        while(true) {
            // recv() Receives data from the client
            char recvBuf[1024];
            int recvBuflen = sizeof(recvBuf);
            ZeroMemory(recvBuf, 1024);

            int bytesRecv = recv(acceptSocket, recvBuf, recvBuflen, 0);
            if (bytesRecv > 0) {
                // Handling request on server
                std::string input(recvBuf, bytesRecv);

                std::cout << "(User input): [" << input << "]\n";
                std::cout << "Recieved bytes from user: [" << bytesRecv << "]\n";
                std::string response{};
                // If input is correct
                if (m_redis.parser(input)) {
                    response = m_redis.executeValidCmd(Log::Logging);
                    std::cout << "(Response to the user): [" << response << "]\n";
                    m_redis.clearCurrCmd();
                } else {
                    response = "Unknown command";
                    std::cout << '\n';
                    m_redis.clearCurrCmd();
                }

                // Sending back to the client response
                int bytes_sent = send(acceptSocket, response.c_str(), response.size(), 0);
                if (bytes_sent == SOCKET_ERROR) {
                    // If sending fails, print an error
                    std::cerr << "send failed: " << WSAGetLastError() << std::endl;
                } else {
                    // Print the number of bytes sent
                    std::cout << "Sent bytes to the user: [" << bytes_sent << "] " << std::endl;
                }

                input.clear();
            } else {
                // If no data is received, print an error message
                std::cout << "recv failed: " << WSAGetLastError() << '\n';
                std::cout << "User disconnected" << '\n';
                break;
            }
        }
    }

    WSACleanup();
    return 1;
}