
#ifndef CLIENT_H
#define CLIENT_H

#include "../packet/Packet.h"
#include "ClientNetwork.h"

#include <fstream>
#include <nlohmann/json.hpp>

#include <iostream>
#include <string>
#include <thread>
#include <vector>

using json = nlohmann::json;

static std::string PATH_TO_ID = "../client/client.json";
static bool RUNNING = 1;

class Client {
    private:

    /* Unique identifier of Client*/
    std::string id;
    
    /* Client's ID */
    std::string client_input;
    
    /* Servers for serialization*/
    std::vector<char> buffer;

    /* Recieving thread */
    std::thread reciever_thread;

    /* Network handler */
    ClientNetwork& network;

public:
    Client(ClientNetwork &net);
    void setup();
    void run();
    void fetchClientInfo();
    void sendGreetings();
    void receive_message();
        
    ssize_t recv_all(size_t length);

};

#endif