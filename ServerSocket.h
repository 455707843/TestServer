#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

#include "Socket.h"
#include <list>
#include <stdio.h>
#include <mysql.h>
#include <sys/epoll.h>
#include <queue>

using std::list;
using std::queue;

#define EPOLL_SIZE 10000
#define FD_SIZE (1024 + 1)
#define MSG_LEN (255 + 255)
#define BUFF_SIZE 5

struct User
{
    char ipAddress[20];
    int port;
};

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
        static void* ProcessInsert(void* arg);
        list<int> clientSockfds;
        struct epoll_event ev, events[EPOLL_SIZE];
        char sendStr[FD_SIZE][MSG_LEN];
        char query[FD_SIZE][MSG_LEN];
        User users[FD_SIZE];
        static MYSQL mysql;
        static queue <char*> messageStore[FD_SIZE];
};

#endif
