
#include "Client.h"

Client::Client(ClientNetwork &net, string path) 
    : network(net), PATH_TO_ID(path) {}

/* Connects client to server address */
int Client::setup(){

    if (network.connectToServer() < 0) {
        cerr << "[Server] - Failed to Connect to server\n";
        return -1;
    }

    fetchClientInfo();
    sendGreetings();

    // Spawn thread listenning to server
    reciever_thread = thread(&Client::receive_message, this);
    return 1;
}

// Main input loop
void Client::run(){
    while(RUNNING){        
        cout << "[" << id << "]> ";
        getline(cin, Client::client_input);

        if (client_input == "\\kill"){
            RUNNING = false;
            break;
        }
        
        Packet p(MessageType::TEXT, id, 999, client_input);
        sendPacket(p);
    }

    network.shutdownSocket();
    reciever_thread.join();
    network.closeSocket();
}

int Client::sendPacket(Packet message){
    buffer = message.serialize();
    if (network.sendBuffer(buffer) < 0){
        cerr << "Failed to send Message";
        return -1;
    }
    return 1;
}

int Client::sendGreetings(){
    string id_msg = "Hello Server ! This is " + to_string(id);
    Packet p(MessageType::GREETINGS, id, 0, id_msg);
    vector<char> buffer = p.serialize();
    if (network.sendBuffer(buffer) < 0) {
        cerr << "Failed to send Greetings";
        return -1;
    }
    return 1;
}

/* Listens for incoming message */
void Client::receive_message(){
    while (RUNNING) {
        Packet p;
        buffer.resize(Packet::HEADER_SIZE);
        
        // // Receive header
        buffer = network.receiveBuffer(Packet::HEADER_SIZE);
        if (buffer.empty()) {
            if (RUNNING) cout << "\r\33[2k" << "Disconnected from server..." << endl;
            else cout << "Terminanting the session... Cya !" << endl;
            RUNNING = false;
            break;
        }
        p.deserializeHeader(buffer);
        
        p.seeHeader();
        cout << "Server said dest=" << p.getDestinationID();
        
        // Recieve Payload
        buffer.resize(p.getPayloadSize());
        buffer = network.receiveBuffer(p.getPayloadSize());
        p.deserializePayload(buffer);
        
        // Display message
        cout << "\r\33[2k" << "[" << p.getSourceID() <<"]> ";
        p.seePayload(); 
        cout << "[" << id << "]> " << flush;
    } 
}

/* Reads client.json and fetches saved credentials */
void Client::fetchClientInfo(){

    filesystem::path filePath(PATH_TO_ID);
    filesystem::create_directories(filePath.parent_path());


    json data;

    // Attemp to read file contents
    ifstream file_in(PATH_TO_ID);
    if (file_in.is_open()) {
        file_in >> data;
        cout << data << endl;
        file_in.close();
    }

    // File is fresh, no id defined
    if (!data.contains("id")){

        requestAndSetNewID();
        data["id"] = this->id;

        // Save JSON to file
        ofstream file_out(PATH_TO_ID);
        file_out << data;
        file_out.close();
    } else {
        this->id = data["id"];
    }

    cout << "New ID Assigned - " << id << endl;
}

/* Requests and sets id field for client */
int Client::requestAndSetNewID(){

    // Request ID From Server
    string msg = "New client - ID Request";
    Packet p(MessageType::IDENTIFICATION, 0, 1, msg);
    sendPacket(p);

    // Thread not spawned here... Recieve manually
    vector<char> buffer = network.receiveBuffer(Packet::HEADER_SIZE);
    p.deserializeHeader(buffer);
    buffer = network.receiveBuffer(p.getPayloadSize());
    p.deserializePayload(buffer);

    // Server have set destID as our newID
    id = p.getDestinationID();
    return 1;
}