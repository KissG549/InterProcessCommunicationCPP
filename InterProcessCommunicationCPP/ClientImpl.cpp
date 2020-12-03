#include "ClientImpl.h"
#include <iostream>
#include <thread>

/*
  Based on
    https://www.winsocketdotnetworkprogramming.com/winsock2programming/winsock2advancedcode1d.html
*/

ClientImpl::ClientImpl()
    :mSocket(),mServerAddr(),mSenderInfo(),mBuffer("")
{
    mFutureObj = mExitSignal.get_future();
}

ClientImpl::~ClientImpl()
{
}

void ClientImpl::run(std::string& pServerAddr, int pPort)
{
    std::cout << "Connecting to the server: "
        << pServerAddr
        << ":"
        << pPort
        << std::endl;

    while (!connect(pServerAddr, pPort))
    {
        /* Wait 1 second before reconnect */
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        std::cout << "Trying to connect to the server: " 
            << pServerAddr 
            << ":" 
            << pPort 
            << std::endl;
    }

    /* Receive packets on independent thread */
    std::thread receiverTh( ClientImpl::receive, this);

    send("Sample message");

    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    mExitSignal.set_value();

    std::cout << "Ask receiver thread to stop." << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(10000));

    std::cout << "Receiver thread timed out." << std::endl;

    receiverTh.detach();

    /* Close the connection */
    disconnect();
}

bool ClientImpl::connect(std::string pServerAddr, int pPort)
{
    return false;
}

void ClientImpl::disconnect()
{
}

void ClientImpl::receive(ClientImpl* pClient)
{
    while (pClient->mFutureObj.wait_for(std::chrono::milliseconds(10)) == std::future_status::timeout )
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

int ClientImpl::send(std::string pMessage)
{
    return 0;
}
