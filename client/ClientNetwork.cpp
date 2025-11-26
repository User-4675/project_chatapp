#include "ClientNetwork.h"

ClientNetwork::ClientNetwork(int port, const std::string &ip)
    : port(port), ip(ip) {}

int ClientNetwork::connectToServer(){

    // Create a socket fd for client
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0){
        perror("[Network] - Failed to create socket");
        return -1;
    }

    // Configure server address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    if (inet_pton(AF_INET, ip.c_str(), &server_address.sin_addr) <= 0){
        perror("[Network] - Failed to setup IP Address");
        return -1;
    }
    
    // Connect client to server
    if (connect(sock_fd, (const sockaddr *)&server_address, 
        sizeof(server_address)) < 0){ 
            perror("[Network] - Failed to connect to server");
            return -1;
    }

    return 1;
}

/* Sends buffer to configured sock fd */
ssize_t ClientNetwork::sendBuffer(const std::vector<char> &buffer){
    size_t total = 0;
    size_t length = buffer.size();
    char *ptr = (char*)buffer.data();

    while (total < length) {
        ssize_t bytes = send(sock_fd, ptr + total, length - total, 0);

        if (bytes < 0) {
            perror("[Network] - Failed to send buffer");
            return -1;
        } else if (bytes == 0){
            std::cerr << "[Network] - Server Closed Connection\n";
            return total;
        }

        total += bytes;
    }
    
    return total;
}

/* Returns recieved data buffer (empty buffer when error) */
std::vector<char> ClientNetwork::receiveBuffer(size_t length){
    std::vector<char> buffer(length);
    ssize_t bytes = recv_all(buffer, length);
    if (bytes <= 0) buffer.clear(); 
    return buffer; // Returns empty buffer if client disconnected
}

/* Ensures that full stream is collected */
ssize_t ClientNetwork::recv_all(const std::vector<char> &buffer, size_t length){
    size_t total = 0;
    ssize_t bytes;

    char* ptr = (char*)buffer.data();
    while (total < length) {
        bytes = recv(sock_fd, ptr + total, length - total, 0);
        
        if (bytes < 0) {
            perror("[Network] - Failed to recieve a message");
            return -1;
        } else if (bytes == 0) {
            std::cerr << "[Network] - Server Closed Connection\n";
            return bytes;
        }
        
        total += bytes;
    }
    return total; // Returns 0 if client disconnected
}

void ClientNetwork::shutdownSocket(){shutdown(sock_fd, SHUT_RDWR);}
void ClientNetwork::closeSocket(){close(sock_fd);}