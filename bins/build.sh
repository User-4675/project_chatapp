#!/bin/bash

echo "Compiling client 1..."
g++ ../client/*.cpp ../packet/*.cpp -o client.out

echo "Compiling client 2..."
g++ ../client2/*.cpp ../packet/Packet.cpp -o client2.out

echo "Compiling server..."
g++  ../server/*.cpp ../packet/Packet.cpp -o server.out
echo "Done !"
