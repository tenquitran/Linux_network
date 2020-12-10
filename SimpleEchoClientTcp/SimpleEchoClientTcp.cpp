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


int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage:\n$ echoclient <ip_address> [<port>]" << std::endl;
        return 1;
    }

    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (-1 == s)
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
    
    if (-1 == connect(s, (sockaddr *)&srvAddr, sizeof(srvAddr)))
        {Utils::err_exit("connect() failed");}
    
    std::cout << "Connected. Starting echo exchange..." << std::endl;
    
    char echoStr[] = "Some string";
    
    size_t cbEcho = strlen(echoStr);

    //////////////////////////////////////////////////////////////////////
    // Send echo string.
    //////////////////////////////////////////////////////////////////////
    
    ssize_t cbSent = send(s, echoStr, cbEcho, 0);
    
    if (-1 == cbSent)
        {Utils::err_exit("send() failed");}
    else if (cbEcho != cbSent)
        {Utils::err_exit("send() sent wrong number of bytes");}

    std::cout << "Sent echo string" << std::endl;
    
    //////////////////////////////////////////////////////////////////////
    // Receive echo string.
    //////////////////////////////////////////////////////////////////////

    std::unique_ptr<char[]> buffRecv  = std::make_unique<char[]>(cbEcho + 1);
    
    ssize_t cbReceivedTotal = {};
    
    while (cbReceivedTotal < cbSent)
    {
        ssize_t cbReceived = recv(s, buffRecv.get(), cbEcho, 0);
        
        if (-1 == cbReceived)
            {Utils::err_exit("recv() failed");}

        buffRecv[cbEcho] = '\0';
        
        printf("%s", buffRecv.get());

        cbReceivedTotal += cbReceived;
    }
    
    std::cout << "Finished. Stopping the client..." << std::endl;

    shutdown(s, SHUT_RDWR);
    close(s);

    return 0;
}
