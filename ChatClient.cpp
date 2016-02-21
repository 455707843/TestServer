#include <iostream>
#include <string>
#include "ClientSocket.h"
#include "SocketException.h"

using namespace std;

int main()
{
    cout << "Running ChatClient..." << endl;
    ClientSocket clientSocket("127.0.0.1", 8080);
    string sendMessage;

    while (true)
    {
        try
        {
            string message;
            clientSocket.Receive(message);
            cout << "Response from server: " << message << endl;
            cin >> sendMessage;
            clientSocket.Send(sendMessage);
            if ("exit" == sendMessage)
            {
                break;
            }
        }
        catch(SocketException& ex)
        {
            cout << "Exception was caught:" << ex.Description() << endl;
        }
    }
    return 0;
}
