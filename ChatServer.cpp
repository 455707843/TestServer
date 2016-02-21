#include <iostream>
#include <string>
#include "ServerSocket.h"
#include "SocketException.h"

using namespace std;

int main()
{
    cout << "Running ChatServer..." << endl;
    try
    {
        ServerSocket server(8080);

        while (true)
        {
            server.Accept();
        }
    }
    catch(SocketException& ex)
    {
        cout << "Exception was caught:" << ex.Description() << "\nExiting.\n";
    }
    return 0;
}