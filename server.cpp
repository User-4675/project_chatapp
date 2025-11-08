#include "Packet.h"

#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <vector>
using namespace std;

const int PORT = 5555;
const int BACKLOG = 5;
const int MAX_CLIENTS = 10;

bool recieveMessage(int sockfd, Packet &p);
ssize_t recv_all(int sock, void* buffer, size_t length);
bool acceptNewClient(int server_fd, int client_sockets[]);
bool setup(int &server_fd, struct sockaddr_in &address, int client_sockets[]);



int main(){

    fd_set readfds;
    vector<char> buffer;
    struct sockaddr_in address; // Data type that stores address of socket
    int server_fd, client_sockets[MAX_CLIENTS], max_sd, activity, valread;

    if(!setup(server_fd, address, client_sockets)){
        return -1;
    }

    cout << "Server listening on port " << PORT << " ..." << endl;
    socklen_t addrlen = sizeof(sockaddr);
    
    while (true) {
        
        FD_ZERO(&readfds); // Set bits of mask to zero
        FD_SET(server_fd, &readfds); // Save server_fd 
        
        // Add all clients to the set
        max_sd = server_fd;
        for (int i = 0; i < MAX_CLIENTS; i++){
            int sd = client_sockets[i];
            if (sd > 0) FD_SET(sd, &readfds); // Add active client to to watchlist
            max_sd = sd > max_sd ? sd : max_sd; // Track the highest fd
        }

        // Listen for activity on any socket
        activity = select(max_sd + 1, &readfds, nullptr, nullptr, nullptr);
        
        if (activity < 0 && (errno != EINTR) ) {
            perror("Select error");
            return 1;
        }

        /* Identify new connections */
        if (FD_ISSET(server_fd, &readfds)) {
            if(!acceptNewClient(server_fd, client_sockets)){
                return -1;
            }
        }

        /* Communicate message between clients */
        for (int i = 0; i < MAX_CLIENTS; i++){
            int curent_socket = client_sockets[i];
            if (FD_ISSET(curent_socket, &readfds)){
                Packet p;
                if(!recieveMessage(curent_socket, p)){
                    cout << "Client " << curent_socket << " disconected\n"; 
                    close(curent_socket);
                    client_sockets[i] = 0;
                    continue;
                }
            }
        }
    }
    close(server_fd);
    return 0;
}

bool acceptNewClient(int server_fd, int client_sockets[]){
    vector<char> buffer;
    struct sockaddr_in address;
    socklen_t addres_len = sizeof(address);

    int new_socket = accept(server_fd, (sockaddr*)&address, &addres_len); 
    if (new_socket < 0){
        perror("Failed to accept Client.");
        return false;
    }

    cout << "New connection: socket fd " << new_socket
            << " - IP " << inet_ntoa(address.sin_addr)
            << " Port " << ntohs(address.sin_port) << endl;

    // Send welcome message
    string msg = "Welcome to server!";
    Packet p(MessageType::GREETINGS, 33, msg);
    p.seeHeader();
    buffer = p.serialize();
    send(new_socket, buffer.data(), buffer.size(), 0);

    // Running forloop even when we have max_sd
    // as there could be free sockets
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (client_sockets[i] == 0) {
            client_sockets[i] = new_socket;
            break;
        }
    }
    return true;
}

bool recieveMessage(int sockfd, Packet& p){
    vector<char> buffer(Packet::HEADER_SIZE);

    // Receive header
    if(recv_all(sockfd, buffer.data(), Packet::HEADER_SIZE) <= 0) return false;

    p.deserializeHeader(buffer);
    p.seeHeader();

    buffer.resize(p.getPayloadSize());
    if(recv_all(sockfd, buffer.data(), p.getPayloadSize()) <= 0) return false;
    
    p.deserializePayload(buffer);
    cout << "Message: ";
    p.seePayload();
    return true;
}

/* Function makes sure that full stream is collected */
ssize_t recv_all(int sock, void* buffer, size_t length){
    size_t total = 0;
    ssize_t bytes;
    char* ptr = (char*)buffer;

    while (total < length) {
        bytes = recv(sock, ptr + total, length - total, 0);
        if (bytes <= 0) return bytes;
        total += bytes;
    }
    return total;
}

bool setup(int &server_fd, struct sockaddr_in &address, int client_sockets[]){
    // Initialize client sockets to empty
    for (int i = 0; i < MAX_CLIENTS; i++)client_sockets[i] = 0;

    // Set socket level option (server can connect to same 
    // port immediately after shutting off)
    int opt = 1;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Failed to create socket\n");
        return false;
    }
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    address.sin_family = AF_INET;  // Address is IPv4
    address.sin_port = htons(PORT); // Convert port to network byte order
    address.sin_addr.s_addr = INADDR_ANY; // Accept connection to any address
    
    
    if (bind(server_fd, (const sockaddr *)&address, sizeof(address))){
        perror("Failed to bind socket with address\n");
        return false;   
    }
    
    if (listen(server_fd, BACKLOG) == -1){
        perror("Server Failed to Listen");
        return false;
    }

    return true;
}