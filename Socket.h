#ifndef SOCKET_H
#define SOCKET_H

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>

const int MAXCONNECTION = 5;
const int MAXRECEIVE = 500;

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
        
        static bool Send(Socket& socket, const std::string& message);
        static int Receive(Socket& socket, std::string& message);

        unsigned int GetAddress();
        int GetPort();
        void SetNonBlocking(const bool flag);
        bool IsValid() const;

    private:
        int m_sockfd;
        struct sockaddr_in m_address;
};

#endif
