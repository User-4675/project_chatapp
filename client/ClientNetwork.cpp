#include "ClientNetwork.h"

ClientNetwork::ClientNetwork(int port, char ip[16]){
    this->port = port;
    strcpy(this->ip, ip);
}

int ClientNetwork::connectToServer(){
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0){
        perror("[Network] - Failed to create socket");
        return -1;
    }

    // Configure server address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server_address.sin_addr);
    
    // Connect client to server
    if (connect(sock_fd, (const sockaddr *)&server_address, 
        sizeof(server_address)) < 0){ 
            perror("[Network] - Failed to connect to server");
            return -1;
        }
    return 1;
}

void ClientNetwork::sendBuffer(std::vector<char> buffer){
    send(sock_fd, buffer.data(), buffer.size(), 0);
}

/* Returns data buffer (empty buffer when error) */
std::vector<char> ClientNetwork::receiveBuffer(size_t length){
    std::vector<char> buffer(length);
    ssize_t bytes = recv_all(buffer.data(), length);
    if (bytes <= 0) buffer.clear(); 
    return buffer; // Returns empty buffer if client disconnected
}

/* Ensures that full stream is collected */
ssize_t ClientNetwork::recv_all(void *buffer, size_t length){
    size_t total = 0;
    ssize_t bytes;
    char* ptr = (char*)buffer;
    while (total < length) {
        bytes = recv(sock_fd, ptr + total, length - total, 0);
        if (bytes <= 0) return bytes;
        total += bytes;
    }
    return total; // Returns 0 if client disconnected
}

void ClientNetwork::shutdownSocket(){shutdown(sock_fd, SHUT_RDWR);}
void ClientNetwork::closeSocket(){close(sock_fd);}