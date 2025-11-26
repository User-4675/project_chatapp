#include "Server.h"

// Bootstart the Server -> Set the server network
Server::Server(Network& net) : serverNetwork(net) {}

bool Server::setup(){
    if (!serverNetwork.serverSetup()){
        printf("[Server] - Failed to set up the server...");
        return false;
    }
    return true;
}

bool Server::run(){

    cout << "Server listening on port " << serverNetwork.getPort() << "...\n";
    while (true) {

        // Reset the fd set to zero and set server fd
        serverNetwork.prepareReadSet();

        // Listen for activity on any socket
        if(serverNetwork.waitForActivity() < 0){
            perror("[Server] - Failed while waiting for activity");
            exit(EXIT_FAILURE);
        }
        

        /* Identify new connections */
        int new_socket = serverNetwork.handleNewClient();
        
        // Retrieve all online clients and react to their messages
        auto ready_sock = serverNetwork.getReadyClients();
        for (int sock : ready_sock){
            Packet msg;
            if(!recieveMessage(sock, msg)){

                cout << "Client " << sockfd_to_id[sock]; 
                cout << " " << sock << " disconected\n"; 
                
                deregisterClient(sock);
                seeOnlineClients();
                continue;
            }

            if (msg.getType() == MessageType::GREETINGS) seeOnlineClients();
        }
    }
    
}

bool Server::recieveMessage(int sockfd, Packet& p){
    vector<char> buffer(Packet::HEADER_SIZE);
    
    // Recieve header 
    buffer = serverNetwork.receiveBuffer(sockfd, Packet::HEADER_SIZE);

    // In case of Identification -> Send only header, server knows
    // ...
    if (buffer.empty()) return false; // Client Disconnected
    p.deserializeHeader(buffer);
    
    // Receive payload 
    buffer.resize(p.getPayloadSize());
    buffer = serverNetwork.receiveBuffer(sockfd, p.getPayloadSize());
    if (buffer.empty()) return false;
    p.deserializePayload(buffer);
    
    
    if (p.getType() == MessageType::GREETINGS){
        // Mark that client exists
        registerClient(sockfd, p.getSourceID());
        return true;
    }
    
    if (p.getType() == MessageType::IDENTIFICATION) {
        // Client is asking for new ID -> Generate it and send back
        uint32_t newID = getNextFreeId();
        string newID_str = to_string(newID);
        cout << "Generated ID - " <<  newID << " for " << sockfd <<endl;
        Packet id_packet(MessageType::IDENTIFICATION, server_id, newID, newID_str);
        buffer = id_packet.serialize();
        send(sockfd, buffer.data(), buffer.size(), 0);
        return true;
    }
    
    // Message is simple text message -> Read and redirect to client
    cout << "[" << sockfd_to_id[sockfd] << " -> " << p.getDestinationID() << "]:";
    p.seePayload();
    // Send the package to sockfd mapped with destionation ID
    if (id_to_client.find(p.getDestinationID()) == id_to_client.end()){
        cout << "Client " << p.getDestinationID() <<" Is not Online" << endl;
        // Check if client exists
    } else {
        serverNetwork.sendBuffer(id_to_client[p.getDestinationID()].sockfd, p.serialize());
    }
    return true;
}



void Server::sendPacket(int sock_fd, MessageType type, uint32_t dest, string msg){
    Packet p = {type, 0, dest, msg};
    vector<char> buffer = p.serialize();
    serverNetwork.sendBuffer(sock_fd, buffer);
}

void Server::registerClient(int sockfd, uint32_t clientID){
    cout << "Registering new client " << clientID;
    id_to_client[clientID] = ClientInfo{clientID, sockfd};
    sockfd_to_id[sockfd] = clientID;
}

void Server::deregisterClient(int sockfd){
    id_to_client.erase(sockfd_to_id[sockfd]);
    sockfd_to_id.erase(sockfd);
    serverNetwork.disconnectClient(sockfd);
    close(sockfd);
}

void Server::seeOnlineClients(){
    cout << "--- Online Client(s) ---\n"; 
    for (auto it = sockfd_to_id.begin(); it != sockfd_to_id.end(); ++it){
        cout << it->second << " (" << it->first << ") ";
    }
    cout << endl << "-------------\n";
}

uint32_t Server::getNextFreeId(){
    return (NEXT_FREE_ID++);
}