#include "../libnet/LibNet.h"
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

//////////////////////////////////////////////////////////////////////

using namespace LibNet;

//////////////////////////////////////////////////////////////////////

int g_srvSocket;

const int BackLog = 5;

const size_t CbBuffer = 256;    // receive buffer size

void sigHandler(int signal);

//////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    signal(SIGINT, sigHandler);
    
    if (argc < 2)
    {
        std::cout << "Usage:\n$ echoserver <port>" << std::endl;
        return 1;
    }
    
    const unsigned short port = atoi(argv[1]);
    
    if (0 == port)
        {Utils::err_exit("Failed to convert port number");}
    
    g_srvSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (-1 == g_srvSocket)
        {Utils::err_exit("Failed to create socket");}
    
    sockaddr_in srvAddress = {};
    srvAddress.sin_family = AF_INET;
    srvAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    srvAddress.sin_port = htons(port);
    
    if (-1 == bind(g_srvSocket, (sockaddr *)&srvAddress, sizeof(srvAddress)))
        {Utils::err_exit("bind() failed");}
    
    if (-1 == listen(g_srvSocket, BackLog))
        {Utils::err_exit("listen() failed");}
    
    while (true)
    {
        sockaddr_in clientAddress = {};
        
        socklen_t addrLen = sizeof(clientAddress);

        int clientSocket = accept(g_srvSocket, (sockaddr *)&clientAddress, &addrLen);
        
        if (-1 == clientSocket)
        {
            if (-1 == g_srvSocket)
            {
                // Our signal handler closed the server socket.
                std::cout << "Server socket has been closed" << std::endl;
                return 0;
            }
            else
                {Utils::err_exit("accept() failed");}
        }
        
        std::cout << "Client accepted: " << inet_ntoa(clientAddress.sin_addr) << std::endl;
        
        // Handle the client.
        
        char rcvBuffer[CbBuffer];
        ssize_t cbReceived = {};
        
        cbReceived = recv(clientSocket, rcvBuffer, CbBuffer, 0);
        
        if (-1 == cbReceived)
            {Utils::err_exit("recv() failed (1)");}
        
        while (cbReceived > 0)
        {
            if (cbReceived != send(clientSocket, rcvBuffer, cbReceived, 0))
                {Utils::err_exit("send() failed");}
            
            cbReceived = recv(clientSocket, rcvBuffer, CbBuffer, 0);
            
            if (-1 == cbReceived)
                {Utils::err_exit("recv() failed (2)");}
            
            std::cout << "Message received and sent successfully" << std::endl;
        }
        
        shutdown(clientSocket, SHUT_RDWR);
        close(clientSocket);
    }
    
    return 0;
}

void sigHandler(int signal)
{
    if (SIGINT == signal)
    {
        std::cout << "SIGINT arrived. Stopping the server..." << std::endl;
        
        shutdown(g_srvSocket, SHUT_RDWR);
        close(g_srvSocket);
        
        g_srvSocket = -1;
    }
    else
        {std::cout << "An unknown signal arrived: " << signal << std::endl;}
}
