
#ifndef NETWORK_H
#define NETWORK_H

#include <stdio.h>
#include <arpa/inet.h>
#include <vector>
#include <iostream>
#include <fcntl.h>

const int MAX_CLIENTS = 10;
const int BACKLOG = 5;

class Network {

    private:
        /* File descriptor set tracking active clients*/
        fd_set readfds;
        
        /* Server's socket and port */
        int server_fd, port;

        /* Socket Address Structure */
        struct sockaddr_in address; 

        /* Bytelength of address */
        socklen_t addrlen;

        /* Integer values used for client communication */
        int client_sockets[MAX_CLIENTS], max_sd, valread;

    public:
        /* Constructor */
        Network(int port);

        /* Getters */
        int getPort() const;

        /* Network Options*/
        bool serverSetup();
        void prepareReadSet();
        int waitForActivity();
        int handleNewClient();
        int acceptNewClient();
        std::vector<int> getReadyClients();
        void disconnectClient(int sock_fd);
        
        /* Communication */
        void sendBuffer(int sock_fd, std::vector<char> buffer);
        std::vector<char> receiveBuffer(int sock_fd, size_t length);
        ssize_t recv_all(int sock, void* buffer, size_t length);

    };



#endif