#include "Server.h"
#include "Network.h"

int main(){

    Network net = {5555};
    Server s = {net};
    s.setup();
    s.run();
    return 0;
}