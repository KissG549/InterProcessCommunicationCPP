#include "ClientImpl.h"
#include "DataEncoderImpl.h"
#include <iostream>
#include <thread>

// Need to link with Ws2_32.lib
#pragma comment(lib, "ws2_32.lib")
/*
  Based on
    https://www.winsocketdotnetworkprogramming.com/winsock2programming/winsock2advancedcode1d.html
*/

void ClientImpl::run(std::string& pServerAddr, int pPort)
{
    std::cout << "Connecting to the server: "
        << pServerAddr
        << ":"
        << pPort
        << std::endl;

    while (!connectToServer(pServerAddr, pPort))
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
    std::thread receiverTh( &ClientImpl::receive, this);

    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    mExitSignal.set_value();

    std::cout << "Ask receiver thread to stop." << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(10000));

    std::cout << "Receiver thread timed out." << std::endl;

    receiverTh.detach();

    /* Close the connection */
    disconnect();
}

bool ClientImpl::connectToServer(std::string pServerAddr, int pPort)
{
    WSADATA wsaData;
    int error = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (error != 0)
    {
        std::cout << "Could not find a usable version of Winsock.dll! WSAStartup failed with error code: " << error << std::endl;
        return false;
    }

    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        /* Tell the user that we could not find a usable */
        /* WinSock DLL.                                  */
        std::cout << "Could not find a usable version of Winsock.dll" << std::endl;

        WSACleanup();
        return false;
    }

    // std::cout << "Client: Winsock DLL status is " << wsaData.szSystemStatus << std::endl;

    // Create a new socket to make a client connection.
    mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (mSocket == INVALID_SOCKET)
    {
        std::cout << "Socket creation failed! Error code: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return false;
    }

    mServerAddr.sin_family = AF_INET;
    mServerAddr.sin_port = htons(pPort);
    // mServerAddr.sin_addr.s_addr = inet_addr(pServerAddr.c_str());
    inet_pton(AF_INET, pServerAddr.c_str(), &mServerAddr.sin_addr.s_addr);

    int connectReturnValue = connect(mSocket, (SOCKADDR*)&mServerAddr, sizeof(mServerAddr));

    if (connectReturnValue != 0)
    {
        std::cout << "Failed to connect to the server (" 
            << pServerAddr 
            << ":" 
            << pPort 
            << "): " 
            << WSAGetLastError() 
            << std::endl;

        closesocket(mSocket);
        WSACleanup();
        return false;
    }

    std::cout << "Connected to the server ("
        << pServerAddr
        << ":"
        << pPort
        << "): "
        << std::endl;

    getsockname(mSocket, (SOCKADDR*)&mServerAddr, (int*)sizeof(mServerAddr));

    return true;
}

void ClientImpl::disconnect()
{
    closesocket(mSocket);
    /* Call WSACleanup when done using the Winsock dll */
    WSACleanup();
}

void ClientImpl::receive()
{
    while (mFutureObj.wait_for(std::chrono::milliseconds(10)) == std::future_status::timeout )
    {
        const static unsigned bufferSize = 1024;
        // receive messages
        char buffer[bufferSize];

        int bytesReceived = recv(mSocket, buffer, bufferSize, 0);

        // Append to the buffer and process it on otherplace
        if (bytesReceived > 0)
        {
            mRecBuffer.append(buffer);
        }
        else if (bytesReceived == 0)
        {
            std::cout << "Connection closed..." << std::endl;
        }
        else
        {
            std::cout << "Receive error: " << WSAGetLastError();
        }

        DataEncoderImpl::processIncomingMessage(mRecBuffer);

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

int ClientImpl::sendMessage(std::string& pMessage)
{
    int bytesSent = send(mSocket, pMessage.c_str(), pMessage.size(), 0);
    pMessage.clear();

    if (bytesSent == SOCKET_ERROR)
    {
        std::cout << "Can't send message, error: " << WSAGetLastError() << std::endl;
    }

    return bytesSent;
}
