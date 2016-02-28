#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

#include "Socket.h"
#include <list>
#include <stdio.h>
#include <mysql.h>
#include <sys/epoll.h>

using std::list;

#define EPOLL_SIZE 10000

class ServerSocket:public Socket
{
    public:	
        ServerSocket(const int port);
        ServerSocket();
        virtual ~ServerSocket();
        
        void Accept(Socket& socket);
        void Run();
    private:
        void AddClient(int clientSockfd);
        bool ProcessMessage(int clientSockfd);
        void SendMessageToAllUsers(int clientSockfd, const std::string& message);

        list<int> clientSockfds;
        struct epoll_event ev, events[EPOLL_SIZE];
        MYSQL mysql;
        char query[255];
};

#endif
