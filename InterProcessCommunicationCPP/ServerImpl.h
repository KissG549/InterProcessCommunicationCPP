#pragma once
#include "IPCAppBase.h"
#include <WinSock2.h>
#include <future>

class ServerImpl :
    public IIPCAppBase
{
public:
    ServerImpl() :
        mServerSocket(INVALID_SOCKET), 
        mClientSocket(INVALID_SOCKET),
        mServerAddr(),
        mSenderInfo()
    {};
    ~ServerImpl() {};

    void run(std::string& pServerAddr, int pPort) override;

private:

    SOCKET mServerSocket;
    SOCKET mClientSocket;
    SOCKADDR_IN mServerAddr;
    SOCKADDR_IN mSenderInfo;

    std::promise<void> mExitSignal;
    std::future<void> mFutureObj;

    void listening();
    void receive();
    int sendMessage(std::string pMessage);

    void closeConnetions();

};
