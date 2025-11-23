#!/bin/bash

echo "Compiling client 1..."
g++ ../client/client.cpp ../packet/Packet.cpp -o client.out

echo "Compiling Client 2..."
g++ ../client2/client.cpp ../packet/Packet.cpp -o client2.out

echo "Compiling server..."
g++  ../server/main.cpp ../server/Server.cpp ../packet/Packet.cpp -o server.out
echo "Done !"

