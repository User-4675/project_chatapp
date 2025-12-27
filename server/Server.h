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

using namespace std;

const int ID_LEN = 6;
const int PORT = 5555;

typedef struct {
    uint32_t id;
    int sockfd;
}ClientInfo;

class Server {
    private:
    /* Maps Clients ID to Client Struct*/
    unordered_map<uint32_t, ClientInfo> id_to_client;
    
    /* Maps sockfd to client's ID */
    unordered_map<int, uint32_t> sockfd_to_id;

    /* Handles Network Communication */
    Network& serverNetwork;

    /* Sever's unique identifier */
    uint32_t server_id = 0;
    
    /* Holds next free value */
    uint32_t NEXT_FREE_ID = 123;
public:
    /* Server Initialization */
    Server(Network& net);
    bool setup();
    bool run();

    /* Client-server Communication*/
    void registerClient(int sockfd, uint32_t clientID);
    void deregisterClient(int sockfd);
    void seeOnlineClients();
    bool recieveMessage(int sockfd, Packet& p);
    void sendPacket(int sock_fd, MessageType type, uint32_t dest, string msg);
    
    uint32_t getNextFreeId();
};

#endif