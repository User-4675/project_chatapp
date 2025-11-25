
#include "Client.h"

Client::Client(ClientNetwork &net) : network(net){}

/* Connects client to server address */
void Client::setup(){
    network.connectToServer();
    fetchClientInfo();
    sendGreetings();
    // Spawn thread listenning to server
    reciever_thread = std::thread(&Client::receive_message, this);
}

// Main input loop
void Client::run(){
    while(RUNNING){        
        std::cout << "[" << id << "]> ";
        getline(std::cin, Client::client_input);

        if (client_input == "\\kill"){
            RUNNING = false;
            break;
        }
        
        Packet p(MessageType::TEXT, 67, client_input);
        buffer = p.serialize();
        network.sendBuffer(buffer);
    }

    network.shutdownSocket();
    reciever_thread.join();
    network.closeSocket();
}



void Client::sendGreetings(){
    Packet p(MessageType::GREETINGS, 11111, id);
    std::vector<char> buffer = p.serialize();
    network.sendBuffer(buffer); 
}

/* Listens for incoming message */
void Client::receive_message(){
    while (RUNNING) {
        Packet p;
        buffer.resize(Packet::HEADER_SIZE);
        
        // // Receive header
        buffer = network.receiveBuffer(Packet::HEADER_SIZE);
        if (buffer.empty()) {
            if (RUNNING) std::cout << "\r\33[2k" << "Disconnected from server..." << std::endl;
            else std::cout << "Terminanting the session... Cya !" << std::endl;
            RUNNING = false;
            break;
        }
        p.deserializeHeader(buffer);
        
        p.seeHeader();
        
        // Recieve Payload
        buffer.resize(p.getPayloadSize());
        buffer = network.receiveBuffer(p.getPayloadSize());
        p.deserializePayload(buffer);
        
        // Display message
        std::cout << "\r\33[2k" << "[Server]> ";
        p.seePayload(); 
        std::cout << "[" << id << "]> " << std::flush;
    } 
}

/* Reads client.json and fetches saved credentials */
void Client::fetchClientInfo(){

    std::ifstream file(PATH_TO_ID);
    if (!file.is_open()){
        perror("Failed to open .json file.");
        exit(EXIT_FAILURE);
    }

    json data;
    file >> data;

    // If ID was never generated before...
    if (data["id"] == "None") {
        std::cout << "Requesting ID..." << std::endl;
        // Request ID From Server
        return;
    }

    this->id = data["id"];
}