#pragma once

#include <vector>
#include <sys/socket.h>
#include <unistd.h>

class Server
{
    int fd;
    std::vector<int> clients;
    sockaddr address;

public:
    Server();
    ~Server() { close(fd); }

    void acceptClient();
    void endListen() { close(fd); }
    void recieveMsg(int clientIdx, char* buffer, std::size_t size) const
        { recv(clients[clientIdx], buffer, size, 0); }
    void sendMsg(int clientIdx, const char* buffer, std::size_t size) const
        { send(clients[clientIdx], buffer, size, 0); }
};


class Client
{
    int fd;
    sockaddr address;

public:
    Client();
    ~Client() { close(fd); }

    void recieveMsg(char* buffer, std::size_t size) const
        { recv(fd, buffer, size, 0); }
    void sendMsg(const char* buffer, std::size_t size) const
        { send(fd, buffer, size, 0); }
};
