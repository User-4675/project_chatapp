#ifndef SERVER_H
#define SERVER_H    

#include "../packet/Packet.h"
#include "Network.h"

#include <iostream>
#include <vector>
#include <arpa/inet.h>
#include <unordered_map>
#include <string>
#include <unistd.h>

const int ID_LEN = 6;
const int PORT = 5555;


class Server {
private:
    /* Maps Clients ID to sock*/
    std::unordered_map <std::string, int> id_to_sock_map;

    /* Maps sock to clients ID */
    std::unordered_map <int, std::string> sock_to_id_map;

    /* Handles Network Communication */
    Network& serverNetwork;

public:
    /* Server Initialization */
    Server(Network& net);
    bool setup();
    bool run();

    /* Client-server Communication*/
    void registerClient(int sockfd, std::string clientID);
    void deregisterClient(int sockfd);
    void seeOnlineClients();
    bool recieveMessage(int sockfd, Packet& p);
    void sendPacket(int sock_fd, MessageType type, uint32_t dest, std::string msg);

};

#endif