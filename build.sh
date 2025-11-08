#!/bin/bash

g++ client.cpp Packet.cpp -o client.out
echo "Compiling client"
g++ server.cpp Packet.cpp -o server.out
echo "Compiling server"
echo "Done"

