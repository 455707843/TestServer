#include "ServerSocket.h"
#include "SocketException.h"
#include <pthread.h>
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

list <Socket*> ServerSocket::clientSockets;

ServerSocket::ServerSocket(const int port)
{
    if (!Socket::Create())
    {
        throw SocketException("Could not create server socket.");
    }

    if (!Socket::Bind(port))
    {
        throw SocketException("Could not bind to port.");
    }

    if (!Socket::Listen())
    {
        throw SocketException("Could not listen to socket.");
    }
}


ServerSocket::~ServerSocket()
{
    list <Socket*>::iterator iter;

    for (iter = clientSockets.begin(); iter != clientSockets.end(); iter++)
    {
        delete (*iter);
    }
}


void ServerSocket::Accept(Socket& socket)
{
    if (!Socket::Accept(socket))
    {
        throw SocketException("Could not accept socket.");
    }
}


bool ServerSocket::Accept()
{
    Socket* clientSocket = new Socket;
    Accept(*clientSocket);
    AddClient(clientSocket);

    pthread_t newThread;
    int result = pthread_create(&newThread, NULL, ProcessMessage, static_cast<void*>(clientSocket));
    if (0 != result)
    {
        return false;
    }

    result = pthread_detach(newThread);
    if (0 != result)
    {
        perror("Failed to detach thread");
    }

    return true;
}


void ServerSocket::Run()
{
    while (serviceFlag)
    {
        if (clientSockets.size() >= static_cast<unsigned int>(MAXCONNECTION))
        {
            serviceFlag = false;
        }
        else
        {
            serviceFlag = Accept();
        }
        sleep(1);
    }
}


void* ServerSocket::ProcessMessage(void* arg)
{
    std::string message;
    Socket* clientSocket = static_cast<Socket*>(arg);
    Send(*clientSocket, "Welcome!");

    while (serviceFlag)
    {
        Receive(*clientSocket, message);
        std::string query = "insert into log(ipAddress, port, time, message) values(";
        sprintf(query, "insert into log(ipAddress, port, time, message) values(%d, %d, %d, %s)", clientSocket->GetAddress(), clientSocket->GetPort(), time(0), message);
        m_Mysql(query);
        if ("exit" == message)
        {
            Send(*clientSocket, "user_exit");
            DeleteClient(clientSocket);
            break;
        }
        else
        {
            SendMessageToAllUsers(clientSocket, message);
        }
        sleep(1);
    }
    pthread_exit(NULL);
    return NULL;
}


void ServerSocket::AddClient(Socket* socket)
{
        clientSockets.push_back(socket);

        std::cout << "Now " << clientSockets.size() << " users.." << std::endl;
        std::cout << "New User: " << socket->GetAddress() << " " << socket->GetPort() << std::endl;

}


void ServerSocket::DeleteClient(Socket* socket)
{
        list <Socket*>::iterator iter;
        for (iter = clientSockets.begin(); iter != clientSockets.end(); iter++)
        {
            if ((*iter)->GetAddress() == socket->GetAddress() && (*iter)->GetPort() == socket->GetPort())
            {
                delete (*iter);
                clientSockets.erase(iter);
                std::cout << "Now " << clientSockets.size() << " users.. " << std::endl;
                break;
            }
        }
}


void ServerSocket::SendMessageToAllUsers(Socket* socket, const std::string& message)
{
    list <Socket*>::iterator iter;
    for (iter = clientSockets.begin(); iter != clientSockets.end(); iter++)
    {
        std::cout << (**iter).GetAddress() << " " << socket->GetAddress() << std::endl;
        std::cout << (**iter).GetPort() << " " << socket->GetPort() << std::endl;
        if ((*iter)->GetAddress() != socket->GetAddress() || (*iter)->GetPort() != socket->GetPort())
        {
            Send(**iter, message);
        }
    }
}
