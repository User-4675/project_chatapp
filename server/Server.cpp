#include "Server.h"

// Bootstart the Server -> Set the server network
Server::Server(Network& net) : serverNetwork(net) {}

bool Server::setup(){
    if (!serverNetwork.serverSetup()){
        printf("Server: Failed to set up the server...");
        return false;
    }
    return true;
}

bool Server::run(){

    std::cout << "Server listening on port " << serverNetwork.getPort() << "...\n";

    while (true) {

        // Reset the fd set to zero and set server fd
        serverNetwork.prepareReadSet();

        // Listen for activity on any socket
        if(serverNetwork.waitForActivity() < 0){
            perror("[Server] - Failed while waiting for activity...");
            exit(EXIT_FAILURE);
        }
        

        /* Identify new connections */
        int new_socket = serverNetwork.handleNewClient();
        if (new_socket != -1){
            // Greet new Client
            sendPacket(new_socket, MessageType::GREETINGS, 67, 
                "Welcome to server!");
        }
        
        // Retrieve all online clients and react to their messages
        auto ready_sock = serverNetwork.getReadyClients();
        for (int sock : ready_sock){
            Packet msg;
            if(!recieveMessage(sock, msg)){

                std::cout << "Client " << sock_to_id_map[sock]; 
                std::cout << " " << sock << " disconected\n"; 
                
                deregisterClient(sock);
                seeOnlineClients();
                continue;
            }

            if (msg.getType() == MessageType::GREETINGS) seeOnlineClients();
        }
    }
    
}

bool Server::recieveMessage(int sockfd, Packet& p){
    std::vector<char> buffer(Packet::HEADER_SIZE);
    
    // Recieve header 
    buffer = serverNetwork.receiveBuffer(sockfd, Packet::HEADER_SIZE);
    if (buffer.empty()) return false; // Client Disconnected
    p.deserializeHeader(buffer);
    
    // Receive payload 
    buffer.resize(p.getPayloadSize());
    buffer = serverNetwork.receiveBuffer(sockfd, p.getPayloadSize());
    if (buffer.empty()) return false;
    p.deserializePayload(buffer);
    
    
    if (p.getType() == MessageType::GREETINGS){
        // Mark that client exists
        std::string clientID;
        p.copyPayload(clientID);
        registerClient(sockfd, clientID);
        
        std::cout << "Client " << sock_to_id_map[sockfd] << " online on ";
        std::cout << sockfd << " fd" << std::endl;
        return true;
    }
    
    if (p.getType() == MessageType::IDENTIFICATION) {
        // Client is asking for new ID -> Generate it and send back
        std::string newId = "#NEW_CLIENT";
        std::cout << "Generating ID " << newId << " -> " << sockfd;
        Packet id_packet(MessageType::TEXT, 67, newId);
        buffer = id_packet.serialize();
        send(sockfd, buffer.data(), buffer.size(), 0);
        return true;
    }
    
    // Message is simple text message
    std::cout << "[" << sock_to_id_map[sockfd] << "]:";
    p.seePayload();
    return true;
}

void Server::sendPacket(int sock_fd, MessageType type, uint32_t dest, std::string msg){
    Packet p = {type, dest, msg};
    std::vector<char> buffer = p.serialize();
    serverNetwork.sendBuffer(sock_fd, buffer);
}

void Server::registerClient(int sockfd, std::string clientID){
    sock_to_id_map[sockfd] = clientID;
    id_to_sock_map[clientID] = sockfd;
}

void Server::deregisterClient(int sockfd){
    id_to_sock_map.erase(sock_to_id_map[sockfd]);
    sock_to_id_map.erase(sockfd);
    serverNetwork.disconnectClient(sockfd);
    close(sockfd);
}

void Server::seeOnlineClients(){
    std::cout << "--- Online Client(s) ---\n"; 
    for (auto it = sock_to_id_map.begin(); it != sock_to_id_map.end(); ++it){
        std::cout << it->second << " (" << it->first << ") ";
    }
    std::cout << std::endl << "-------------\n";
}