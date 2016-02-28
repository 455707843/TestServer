#include <iostream>
#include <string>
#include <time.h>
#include "ClientSocket.h"
#include "SocketException.h"

using namespace std;

string sMessage, rMessage;
ClientSocket* client;
bool flag;

void* ProcessReceive(void* arg)
{
    while (flag)
    {
        client->Receive(rMessage);
        cout << rMessage << endl;
    }
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
        while (getline(cin, sMessage))
        {
            if ("exit" == sMessage)
            {
                flag = false;
                delete client;
                break;
            }
            client->Send(sMessage);
            showTime();
        }
    }
    catch(SocketException& ex)
    {
        cout << "Exception was caught:" << ex.Description() << endl;
    }
    return 0;
}
