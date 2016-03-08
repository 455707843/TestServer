#!/bin/sh
g++ -I /usr/include/mysql ChatServer.cpp ServerSocket.cpp Socket.cpp -lpthread -o ChatServer -L/usr/lib64/mysql -lmysqlclient
g++ Socket.cpp ClientSocket.cpp ChatClients.cpp -lpthread -o ChatClients
