#pragma once
#include "IPCAppBase.h"

#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
//
//#include <windows.h>
//#include <WinSock2.h>

#endif // 

class ClientImpl :
    public IIPCAppBase
{
public:
    ClientImpl();
    ~ClientImpl();

    void run(std::string& pServerAddr, int pPort) override;

private:

    //struct sockaddr_in mServerAddr;

    bool connect(std::string pServerAddr, int pPort);
    void disconnect();
    void receive();
    int send(std::string pMessage);
};

