#include "ServerSocket.h"
#include "SocketException.h"
#include <pthread.h>
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

MYSQL ServerSocket::mysql;
queue <char*> ServerSocket::messageStore[FD_SIZE];
bool flag;

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
    
    flag = true;
    SetNonBlocking(true);
}


ServerSocket::~ServerSocket()
{
    flag = false;
    mysql_close(&mysql);  
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
    int epfd;
    int client, epoll_events_count;

    ev.events = EPOLLIN | EPOLLET;
    epfd = epoll_create(EPOLL_SIZE);
    ev.data.fd = GetSockfd();
    epoll_ctl(epfd, EPOLL_CTL_ADD, GetSockfd(), &ev);
    pthread_t newThread;

    int result = pthread_create(&newThread, NULL, ProcessInsert, NULL);

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
                strcpy(users[clientSock->GetSockfd()].ipAddress, clientSock->GetAddress());
                users[clientSock->GetSockfd()].port = clientSock->GetPort();
                AddClient(clientSock->GetSockfd());
                Send(clientSock->GetSockfd(), "Welcome!");
            }
            else
            {
                ProcessMessage(events[i].data.fd);
            }
        }
    }
    close(epfd);
}


bool ServerSocket::ProcessMessage(int clientSockfd)
{
    std::string message = "";
    std::string rmessage = "";
    std::string smessage = "";
    while (Receive(clientSockfd, rmessage))
    {
        int len = rmessage.length();
        if (0 == rmessage.length())
        {
            close(clientSockfd);
            clientSockfds.remove(clientSockfd);
        }
        for (int i = 0; i < len - 3; i++)
        {
            message += rmessage[i];
            if (rmessage[i + 1] == '@' && rmessage[i + 2] == '$' && rmessage[i + 3] == '#')
            {
                i += 3;
    	        time_t now;
    	        struct tm *timenow;
    	        time(&now);
    	        timenow = localtime(&now);
    	        sprintf(query[clientSockfd], "insert into log(ipAddress, port, time, message) values('%s', '%d', '%s', '%s');", 
                                         users[clientSockfd].ipAddress, users[clientSockfd].port, asctime(timenow), &message[0]);

    	        messageStore[clientSockfd].push(query[clientSockfd]);
    	        if (0 == message.length())
    	        {
        	        std::cout << asctime(timenow) << "IP:" << users[clientSockfd].ipAddress << " Port:" << users[clientSockfd].port << std::endl;
		        std::cout << "quit" << std::endl;
        	        close(clientSockfd);
        	        clientSockfds.remove(clientSockfd);
    	        }
    	        else
    	        {
        	        std::cout << asctime(timenow) << "IP:" << users[clientSockfd].ipAddress << " Port:" << users[clientSockfd].port << std::endl;
        	        std::cout << message << std::endl;
        	        sprintf(sendStr[clientSockfd], "%sIP:%s Port:%d\n%s\n", asctime(timenow), 
			        users[clientSockfd].ipAddress, users[clientSockfd].port, &message[0]);

        	        int len = strlen(sendStr[clientSockfd]);

        	        for (int i = 0; i < len; i++)
        	        {
            		    smessage += sendStr[clientSockfd][i];
        	        }
        	        SendMessageToAllUsers(clientSockfd, smessage);
    	        }
                message = "";
            }
        }
    }

    return true;
}

void* ServerSocket::ProcessInsert(void* arg)
{
    mysql_init(&mysql);

    if (!mysql_real_connect(&mysql, "localhost", "root", "root", "testdb", 0, NULL, 0))
    {
        std::cout << mysql_error(&mysql) << std::endl;
    }
    else
    {
        std::cout << "Conected to mysql!" << std::endl;
    }

    while (flag)
    {
        for (int i = 0; i < FD_SIZE; i++)
        {
            if (messageStore[i].size() >= BUFF_SIZE)
            {
                char* cur = messageStore[i].front();
                int res = mysql_query(&mysql, cur);

                if (0 != res)
                {
                    std::cout << "Insert failed!" << std::endl;
                }
                messageStore[i].pop();
            }
        }
    }
    for (int i = 0; i < FD_SIZE; i++)
    {
        while (!messageStore[i].empty())
        {
            char* cur = messageStore[i].front();
            int res = mysql_query(&mysql, cur);
            if (0 != res)
            {
                std::cout << "Insert failed!" << std::endl;
            }
            messageStore[i].pop();
        }
    }
    pthread_exit(NULL);
}

void ServerSocket::AddClient(int clientSockfd)
{
    clientSockfds.push_back(clientSockfd);
    std::cout << "Now " << clientSockfds.size() << " users.." << std::endl;
    std::cout << "New user " << "IP:" << users[clientSockfd].ipAddress << "  Port:" << users[clientSockfd].port << std::endl;
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
