
#include "Client.h"
#include "ClientNetwork.h"

int main(){
    char server_ip[] = "127.0.0.1";
    ClientNetwork net = {5555, server_ip};
    Client local_client = {net};
    
    local_client.setup();
    local_client.run();
    return 0;
}
