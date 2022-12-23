#include "tcp_connection.hpp"

#include <stdexcept>
#include <arpa/inet.h>

constexpr int minPort = 7090;
constexpr int maxPort = 7100;

// TODO remove me pls
sockaddr_in recentServerAddress {};

Server::Server()
{
    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (fd < 0)
        throw std::runtime_error("Server socket creation failed");
    int port;
    for (port = minPort; port < maxPort; ++port)
    {
        recentServerAddress = {
            AF_INET,              // sin_family;
            htons(port),          // sin_port;
            {htons(INADDR_ANY)},  // sin_addr;
            {}                    // sin_zero
        };
        address = *reinterpret_cast<sockaddr*>(&recentServerAddress);
        if (bind(fd, &address, sizeof(address)) >= 0)
            break;
    }
    if (port == maxPort)
        throw std::runtime_error("Server socket bind failed");
    listen(fd, SOMAXCONN);
}

void Server::acceptClient()
{
    socklen_t clientSize = sizeof(sockaddr_in);
    clients.push_back(accept(fd, &address, &clientSize));
}

Client::Client()
{
    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (fd < 0)
        throw std::runtime_error("Cilent socket creation failed");
    address = *reinterpret_cast<sockaddr*>(&recentServerAddress);
    if (connect(fd, &address, sizeof(address)) < 0)
        throw std::runtime_error("Client connection failed");
}

