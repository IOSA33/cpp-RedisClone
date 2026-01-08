# Clone of Redis with Network
- Built on std::unordered_map.
- Client-server communication via Winsock2 over TCP protocol.
- AOF Persistence, logging logic for data recovery on restart.
- Commands validation
- TTL Support: Automatic key expiration.

# Commands that you can Run:
- `SET [key] [value] *[EXPIRE]` - Store a string, `[*]` means optionally can pass.
- `GET [key]` - Retrieve a string.
- `EXISTS [key]` - Returns 1 or 0.
- `DEL [key]` - Deletes the key. 
- `exit` - Disconnects from server.

# How to Start:
- Every command can run only in `./client.exe` .
- For graceful shotdown the server write in running `./server.exe` following command `exit` .
- In the main folder run following commands with Cmake for the server:
```
mkdir build
cd build
cmake ..
ninja
```

- For the client run following command in terminal
```
g++ NetWork/Client.cpp -lws2_32 -o client
```

- In the first terminal:
Start the server in the build folder: `./app.exe`
- In the Second terminal run:
Connect the client: `./client.exe`