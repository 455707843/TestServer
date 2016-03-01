#include <iostream>
#include <string>
#include "ClientSocket.h"
#include "SocketException.h"

using namespace std;

int main()
{
    cout << "Running ChatClient..." << endl;
    string sMessage, rMessage;

    try
    {
        for (int i = 1; i <= 10000; i++)
        {
            ClientSocket* client = new ClientSocket("127.0.0.1", 8080);
            client->Receive(rMessage);
            sMessage = "I am ";
            int a = i;
            while (a)
            {
                sMessage += a % 10 + '0';
                a /= 10;
            }
            client->Send(sMessage);
            cout << rMessage << endl;
        }
    }
    catch(SocketException& ex)
    {
        cout << "Exception was caught:" << ex.Description() << endl;
    }
    return 0;
}
