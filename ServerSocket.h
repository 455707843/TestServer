#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

#include "Socket.h"
#include <list>
#include <stdio.h>

using std::list;

class ServerSocket:public Socket
{
    public:	
        ServerSocket(const int port);
        ServerSocket();
        virtual ~ServerSocket();
        
        void Accept(Socket& socket);
        void Run();

        bool Accept();
    private:
        void AddClient(Socket* clientSocket);
        static void DeleteClient(Socket* clientSocket);
        static void* ProcessMessage(void* arg);
        static void SendMessageToAllUsers(Socket* socket, const std::string& message);

        static list<Socket*> clientSockets;
        static bool serviceFlag;
        static Mysql m_Mysql;
};

#endif
