#pragma once
#include "IPCAppBase.h"
#include <future>

#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
//
//#include <windows.h>
#include <WinSock2.h>

#endif // 

class ClientImpl :
    public IIPCAppBase
{
public:
    ClientImpl();
    ~ClientImpl();

    void run(std::string& pServerAddr, int pPort) override;

private:

    /* from WinSock2 */
    SOCKET mSocket;
    SOCKADDR_IN mServerAddr;
    SOCKADDR_IN mSenderInfo;
    /* END form WinSock2 */
    std::string mBuffer;
//    std::atomic_bool mDone;

    std::promise<void> mExitSignal;
    std::future<void> mFutureObj;

    bool connect(std::string pServerAddr, int pPort);
    void disconnect();
    static void receive(ClientImpl* pClient);
    int send(std::string pMessage);
};

