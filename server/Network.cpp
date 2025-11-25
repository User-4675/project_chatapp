
#include "Network.h"

Network::Network(int port){
    this->port = port;
}

bool Network::serverSetup(){

    // Initialize empty client sockets 
    for (int i = 0; i < MAX_CLIENTS; i++) client_sockets[i] = 0;

    // Set socket level option (server can connect to same 
    // port immediately after shutting off)
    int opt = 1;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Failed to create socket\n");
        return false;
    }

    // Make server socket non-blocking
    int flags = fcntl(server_fd, F_GETFL, 0);
    if (flags == -1) { perror("fcntl F_GETFL"); return false; }
    if (fcntl(server_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl F_SETFL O_NONBLOCK");
        return false;
    }
    
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    // Configure server address
    address.sin_family = AF_INET;  // Address is IPv4
    address.sin_port = htons(this->port); // Convert port to network byte order
    address.sin_addr.s_addr = INADDR_ANY; // Accept connection to any address
    
    // Bind the server socket
    if (bind(server_fd, (const sockaddr *)&address, sizeof(address))){
        perror("Failed to bind socket with address");
        return false;   
    }
    
    // Make server listen for traffic
    if (listen(server_fd, BACKLOG) == -1){
        perror("Server Failed to Listen...");
        return false;
    }

    // Set the size of address
    this->addrlen = sizeof(sockaddr);
    return true;
}

void Network::prepareReadSet(){
    
    FD_ZERO(&readfds);
    FD_SET(server_fd, &readfds);

    // Scan and add clients to set
    max_sd = server_fd;
    for (int i = 0; i < MAX_CLIENTS; i++){
        int sd = client_sockets[i];
        if (sd > 0) FD_SET(sd, &readfds); 
        max_sd = sd > max_sd ? sd : max_sd;
    }
}

int Network::waitForActivity(){
    int activity = select(max_sd + 1, &readfds, nullptr, nullptr, nullptr);
    if(activity < 0 && (errno != EINTR)){
        perror("Select Error...");
        return -1;
    }
    return activity;
}

int Network::handleNewClient() {
    int new_socket = -1;
    if (FD_ISSET(server_fd, &readfds)) {
        new_socket = acceptNewClient();
        if(new_socket < 0){
            return -1;
        }
    }
    return new_socket;
}

/* Itterates through sockets and returns first active one.
    Returns -1 if there is not pending client */
std::vector<int> Network::getReadyClients(){
    std::vector<int> ready_clients;

    for(int i = 0; i < MAX_CLIENTS; i++){
        int cur_sock = client_sockets[i];
        if (FD_ISSET(cur_sock, &readfds)) {
            ready_clients.push_back(cur_sock);
        }
    }
    return ready_clients;
}

int Network::acceptNewClient(){
    struct sockaddr_in address;
    socklen_t addres_len = sizeof(address);

    int new_socket = accept(server_fd, (sockaddr*)&address, &addres_len); 
    if (new_socket < 0 && errno != EAGAIN){
        perror("[Network] - Failed to accept Client.");
        return -1;
    }

    std::cout << "New connection: socket fd " << new_socket
            << " - IP " << inet_ntoa(address.sin_addr)
            << " Port " << ntohs(address.sin_port) << std::endl;        

    // Identify max_socket for `accept()`
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (client_sockets[i] == 0) {
            client_sockets[i] = new_socket;
            break;
        }
    }
    return new_socket;
}

int Network::getPort() const {return this->port;}

void Network::sendBuffer(int sock_fd, std::vector<char> buffer){
    send(sock_fd, buffer.data(), buffer.size(), 0);
}

/* Returns data buffer (empty buffer when error) */
std::vector<char> Network::receiveBuffer(int sock_fd, size_t length){
    std::vector<char> buffer(length);
    ssize_t bytes = recv_all(sock_fd, buffer.data(), length);
    if (bytes <= 0) buffer.clear(); 
    return buffer; // Returns empty buffer if client disconnected
}

/* Ensures that full stream is collected */
ssize_t Network::recv_all(int sock, void *buffer, size_t length){
    size_t total = 0;
    ssize_t bytes;
    char* ptr = (char*)buffer;
    while (total < length) {
        bytes = recv(sock, ptr + total, length - total, 0);
        if (bytes <= 0) return bytes;
        total += bytes;
    }
    return total; // Returns 0 if client disconnected
}

/* Removes file descriptor from clients*/
void Network::disconnectClient(int sock_fd){
    for (int i = 0; i < MAX_CLIENTS; i++){
        if(client_sockets[i] == sock_fd){
            client_sockets[i] = 0;
            break;
        }
    }
}

