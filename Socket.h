#ifndef SOCKET_H
#define SOCKET_H

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>

const int MAXCONNECTION = 10000;
const int MAXRECEIVE = 1024;

class Socket
{
    public:
        Socket();
        virtual ~Socket();

        bool Create();
        bool Bind(const int port);
        bool Listen() const;
        bool Accept(Socket& clientSocket) const;

        bool Connect(const std::string& host, const int port);
        
        bool Send(int sockfd, const std::string& message);
        int Receive(int sockfd, std::string& message);

        char* GetAddress();
        int GetPort();
	int GetSockfd();
        void SetNonBlocking(const bool flag);
        bool IsValid() const;

    private:
        int m_sockfd;
        char ipAddress[20];
        struct sockaddr_in m_address;
};

#endif
