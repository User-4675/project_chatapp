
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
    char ip[16];
    /* Servers  */
    struct sockaddr_in server_address;
public:
    ClientNetwork(int port, char ip[16]);
    int connectToServer();
    
    std::vector<char> receiveBuffer(size_t length);
    ssize_t recv_all(void *buffer, size_t length);
    void sendBuffer(std::vector<char> buffer);
    
    void shutdownSocket();
    void closeSocket();
};  

#endif