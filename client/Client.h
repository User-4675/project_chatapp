
#ifndef CLIENT_H
#define CLIENT_H

#include "../packet/Packet.h"
#include "ClientNetwork.h"

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

#include <iostream>
#include <string>
#include <thread>
#include <vector>

using json = nlohmann::json;

// static string PATH_TO_ID = "../client/client.json";
static bool RUNNING = 1;

class Client {
    private:

    /* Unique identifier of Client*/
    uint32_t id;
    
    /* Client's ID */
    string client_input;
    
    /* Servers for serialization*/
    vector<char> buffer;

    /* Recieving thread */
    thread reciever_thread;

    /* Network handler */
    ClientNetwork& network;

    string PATH_TO_ID;

public:
    Client(ClientNetwork &net, string path);
    int setup();
    void run();
    void fetchClientInfo();
    
    int sendGreetings();
    void receive_message();
    int requestAndSetNewID();

    
    int sendPacket(Packet message);
};

#endif