# Clone of Redis with Network
- Built on std::unordered_map.
- Client-server communication via Winsock2 over TCP protocol.
- AOF Persistence, logging logic for data recovery on restart.
- Commands validation
- TTL Support: Automatic key expiration.

# Commands that you can Run:
- SET [key] [value] *[EXPIRE] - Store a string, [*] means optionally can pass.
- GET [key] - Retrieve a string.
- DELETE [key] - Deletes the key. 
- exit - Disconnects from server.

# How to Start:
- Every command can run only in ./client.exe
- In the folder run following commands:
1. g++ main.cpp Redis.cpp Logger.cpp NetWork/Server.cpp -lws2_32 -o app 
2. g++ NetWork/Client.cpp -lws2_32 -o client

- In the first terminal:
1. Start the server: ./app.exe
- In the Second terminal run:
2. Connect the client: ./client.exe