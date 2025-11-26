
#ifndef CLIENT_NETWORK_H
#define CLIENT_NETWORK_H

#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <vector>

class ClientNetwork {
private:
    /* Client's socket and port */
    int sock_fd, port; 
    /* Servers IP address */
    std::string ip;
    /* Serves Address Structure */
    struct sockaddr_in server_address;
public:

    ClientNetwork(int port, const std::string &ip);
    int connectToServer();
    
    std::vector<char> receiveBuffer(size_t length);
    ssize_t sendBuffer(const std::vector<char> &buffer);
    ssize_t recv_all(const std::vector<char> &buffer, size_t length);
    
    void closeSocket();
    void shutdownSocket();
};  

#endif