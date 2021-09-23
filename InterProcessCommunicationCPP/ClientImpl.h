#pragma once
#include "IPCAppBase.h"
#include <future>

#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
    //
    //#include <windows.h>
    #include <WinSock2.h>
    #include <WS2tcpip.h>
#endif // 


class ClientImpl :
    public IIPCAppBase
{
public:
    ClientImpl()
        :mSocket(mSocket), 
        mServerAddr(), 
        mSenderInfo(),
        mReceiveBuffer("")
    {
        mFutureObj = mExitSignal.get_future();
        mSocket = INVALID_SOCKET;
    }

    ~ClientImpl() 
    {
        if (mSocket != INVALID_SOCKET)
        {
            closesocket(mSocket);
            WSACleanup();
        }
    }

    void run(std::string& pServerAddr, int pPort) override;

private:

    SOCKET      mSocket;
    SOCKADDR_IN mServerAddr;
    SOCKADDR_IN mSenderInfo;
    std::string mReceiveBuffer;

    std::promise<void> mExitSignal;
    std::future<void>  mFutureObj;

    bool connectToServer(std::string pServerAddr, int pPort);
    void disconnect();
    void receive();
    int sendMessage(std::string& pMessage);
};

