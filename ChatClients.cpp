#include <iostream>
#include <string>
#include <time.h>
#include "ClientSocket.h"
#include "SocketException.h"

using namespace std;

#define TEST

string sMessage, rMessage;
string span = "@$#";
ClientSocket* client;
bool flag;

void* ProcessReceive(void* arg)
{
    while (flag)
    {
        while (client->Receive(rMessage))
        {
            cout << rMessage << endl;
        }
    }
    pthread_exit(NULL);
}

void showTime()
{
    time_t now;
    struct tm *timenow;
    time(&now);
    timenow = localtime(&now);
    cout << asctime(timenow) << "IP:" << client->GetAddress() << " Port:" << client->GetPort() << endl;
}

int main()
{
    cout << "Running ChatClient..." << endl;
    client = new ClientSocket("127.0.0.1", 8080);
    flag = true;
    
    try
    {
        pthread_t newThread;
        int result = pthread_create(&newThread, NULL, ProcessReceive, NULL);
        if (0 != result)
        {
            return 0;
        }
        showTime();
#ifdef TEST
        for (int i = 1; i <= 1000000; i++)
        {
            sMessage = "";
            int a = i;
            while (a)
            {
                sMessage += (char)(a % 10 + '0');
                a /= 10;
            }
            if ("exit" == sMessage)
            {
                flag = false;
                delete client;
                break;
            }
            cout << sMessage << endl;
            sMessage += span;
            client->Send(sMessage);
            showTime();
        }
#endif
        while (getline(cin, sMessage))
        {
            if ("exit" == sMessage)
            {
                flag = false;
                delete client;
                break;
            }
            sMessage += span;
            client->Send(sMessage);
            showTime();
        }
        close(client->GetSockfd());
    }
    catch(SocketException& ex)
    {
        cout << "Exception was caught:" << ex.Description() << endl;
    }
    return 0;
}
