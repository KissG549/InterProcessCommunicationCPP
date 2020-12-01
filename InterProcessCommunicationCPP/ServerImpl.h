#pragma once
#include "IPCAppBase.h"

class ServerImpl :
    public IIPCAppBase
{
public:
    ServerImpl();
    ~ServerImpl();

    void run(std::string& pServerAddr, int pPort) override;

private:
    void listening();
    void receive();
    int send(std::string pMessage);

};

