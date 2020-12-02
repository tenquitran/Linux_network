#include "../libnet/LibNet.h"
#include <iostream>
#include <cstring>
#include <memory>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

//////////////////////////////////////////////////////////////////////

using namespace LibNet;

//////////////////////////////////////////////////////////////////////

int g_socket;

void sigHandler(int signal);

//////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    signal(SIGINT, sigHandler);
    
    if (argc < 2)
    {
        std::cout << "Usage:\n$ echoclient <ip_address> [<port>]" << std::endl;
        return 1;
    }

    g_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (-1 == g_socket)
        {Utils::err_exit("Failed to create socket");}
    
    const char* srvIp = argv[1];
    
    unsigned short srvPort = 7;

    if (3 == argc)
    {
        srvPort = atoi(argv[2]);
        
        if (0 == srvPort)
            {Utils::err_exit("Failed to convert port number");}
    }
    
    sockaddr_in srvAddr = {};
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr(srvIp);
    srvAddr.sin_port = htons(srvPort);
    
    if (-1 == connect(g_socket, (sockaddr *)&srvAddr, sizeof(srvAddr)))
        {Utils::err_exit("connect() failed");}
    
    std::cout << "Connected. Starting echo exchange..." << std::endl;
    
    char echoStr[] = "Some string";
    
    size_t cbEcho = strlen(echoStr);
    
    size_t attempts = {};    // sending attempts
    size_t failures = {};    // sending failures
    
    while (true)
    {
        //////////////////////////////////////////////////////////////////////
        // Send echo string.
        //////////////////////////////////////////////////////////////////////
        
        ssize_t cbSent = send(g_socket, echoStr, cbEcho, 0);
        
        if (-1 == cbSent)
            {Utils::err_exit("send() failed");}
        else if (cbEcho != cbSent)
            {Utils::err_exit("send() sent wrong number of bytes");}

        ++attempts;
        
        std::cout << "Sent echo string" << std::endl;
        
        //////////////////////////////////////////////////////////////////////
        // Receive echo string.
        //////////////////////////////////////////////////////////////////////
        
        // Temporary buffer used by a single recv() call.
        std::unique_ptr<char[]> buffRecv  = std::make_unique<char[]>(cbEcho + 1);
        
        // Buffer for the entire echo string.
        std::unique_ptr<char[]> buffFinal = std::make_unique<char[]>(cbEcho + 1);
        
        ssize_t cbReceivedTotal = {};
        
        while (cbReceivedTotal < cbSent)
        {
            ssize_t cbReceived = recv(g_socket, buffRecv.get(), cbEcho, 0);
            
            if (-1 == cbReceived)
                {Utils::err_exit("recv() failed");}
            
            strcpy(buffFinal.get() + cbReceivedTotal, buffRecv.get());
            
            cbReceivedTotal += cbReceived;
        }
        
        buffRecv[cbEcho] = '\0';
        
        //////////////////////////////////////////////////////////////////////
        // Check echo string.
        //////////////////////////////////////////////////////////////////////
        
        if (0 != strcmp(buffFinal.get(), echoStr))
            {++failures;}
        
        //////////////////////////////////////////////////////////////////////
        // Display status.
        //////////////////////////////////////////////////////////////////////
        
        if (0 == attempts % 10)
        {
            std::cout << "\nEcho strings sent: " << attempts 
                      << ", failures: " << failures << '\n' << std::endl;
        }
    }

    return 0;
}

void sigHandler(int signal)
{
    if (SIGINT == signal)
    {
        std::cout << "SIGINT arrived. Stopping the client..." << std::endl;
        
        shutdown(g_socket, SHUT_RDWR);
        close(g_socket);
    }
    else
        {std::cout << "An unknown signal arrived: " << signal << std::endl;}
}
