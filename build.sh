#!/bin/bash
rm -R build/
mkdir build
g++ client.cpp -o build/client
g++ server.cpp -o build/server
