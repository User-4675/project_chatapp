#!/bin/bash

echo "Compiling client..."
g++ ../client/client.cpp ../packet/Packet.cpp -o client.out
echo "Compiling server..."
g++ ../server/server.cpp ../packet/Packet.cpp -o server.out
echo "Done !"

