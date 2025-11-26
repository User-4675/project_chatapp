#!/bin/bash

echo "Compiling client..."
g++ ../client/*.cpp ../packet/*.cpp -o client.out

# Remove client data (for id-request testing)
rm -r ../clientJSON/
mkdir ../clientJSON

# Compile and run server if compilation succeeded
echo "Compiling server..."
g++  ../server/*.cpp ../packet/Packet.cpp -o server.out && ./server.out || echo "Server Failed to compile"

