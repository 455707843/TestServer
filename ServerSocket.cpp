#include "ServerSocket.h"
#include "SocketException.h"
#include <pthread.h>
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

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
    
    mysql_init(&mysql);
    if (!mysql_real_connect(&mysql, "localhost", "root", "root", "testdb", 0, NULL, 0))
    {
        printf("%s", mysql_error(&mysql));
    }
    else
    {
        printf("Conected to mysql!\n");
    }
    SetNonBlocking(true);
}


ServerSocket::~ServerSocket()
{
}


void ServerSocket::Accept(Socket& socket)
{
    if (!Socket::Accept(socket))
    {
        throw SocketException("Could not accept socket.");
    }
}

void ServerSocket::Run()
{
    ev.events = EPOLLIN | EPOLLET;
    int epfd;
    int client, epoll_events_count;
    epfd = epoll_create(EPOLL_SIZE);
    ev.data.fd = GetSockfd();
    epoll_ctl(epfd, EPOLL_CTL_ADD, GetSockfd(), &ev);

    while (true)
    {
        epoll_events_count = epoll_wait(epfd, events, EPOLL_SIZE, -1);

        for (int i = 0; i < epoll_events_count; i++)
        {
            if (events[i].data.fd == GetSockfd())
            {
                Socket* clientSock = new Socket;
                Accept(*clientSock);
                clientSock->SetNonBlocking(true);
                ev.data.fd = clientSock->GetSockfd();
                epoll_ctl(epfd, EPOLL_CTL_ADD, clientSock->GetSockfd(), &ev);
                AddClient(clientSock->GetSockfd());
                Send(clientSock->GetSockfd(), "Welcome!");
            }
            else
            {
                ProcessMessage(events[i].data.fd);
            }
        }
    }
}


bool ServerSocket::ProcessMessage(int clientSockfd)
{
    std::string message;
    Receive(clientSockfd, message);
    time_t now;
    struct tm *timenow;
    time(&now);
    timenow = localtime(&now);
    sprintf(query, "insert into log(time, message) values('%s', '%s')", 
                                         asctime(timenow), &message[0]);
    if (0 == message.length())
    {
        close(clientSockfd);
        clientSockfds.remove(clientSockfd);
    }
    else
    {
        
        int res = mysql_query(&mysql, query);
        SendMessageToAllUsers(clientSockfd, message);
    }
    return true;
}


void ServerSocket::AddClient(int clientSockfd)
{
    clientSockfds.push_back(clientSockfd);

    std::cout << "Now " << clientSockfds.size() << " users.." << std::endl;
}

void ServerSocket::SendMessageToAllUsers(int clientSockfd, const std::string& message)
{
    list <int>::iterator iter;
    for (iter = clientSockfds.begin(); iter != clientSockfds.end(); iter++)
    {
        if ((*iter) != clientSockfd)
        {
            Send(*iter, message);
        }
    }
}
