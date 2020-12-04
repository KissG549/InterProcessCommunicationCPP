#include "ServerImpl.h"
#include "DataEncoderImpl.h"
#include <thread>
#include <iostream>
#include <ws2tcpip.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

void ServerImpl::run(std::string& pServerAddr, int pPort)
{
	/* Create the socket */
	WSADATA wsaData;
	// Init winsock
	int error = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (error != 0)
	{
		std::string errorMsg = "Could not find a usable version of Winsock.dll! WSAStartup failed with error code: ";
		errorMsg.append(std::to_string(error));

		throw std::runtime_error(errorMsg);
	}

	SOCKET clientSocket = INVALID_SOCKET;

	static const int bufferLength = 1024;

	int iSendResult = 0;
	char recvbuf[bufferLength]{};

	// Create a SOCKET for connecting to server
	mServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mServerSocket == INVALID_SOCKET) {
		std::string errorMsg = "socket failed with error: ";
		errorMsg.append(std::to_string(WSAGetLastError()));
		WSACleanup();
		throw std::runtime_error(errorMsg);
	}

	struct addrinfo serverAddrInfo;
	ZeroMemory(&serverAddrInfo, sizeof(serverAddrInfo));
	serverAddrInfo.ai_family = AF_INET;
	serverAddrInfo.ai_socktype = SOCK_STREAM;
	serverAddrInfo.ai_protocol = IPPROTO_TCP;
	serverAddrInfo.ai_flags = AI_PASSIVE;
	inet_pton(AF_INET, pServerAddr.c_str(), serverAddrInfo.ai_addr);

	sockaddr_in serverListener;
	serverListener.sin_family = AF_INET;
	serverListener.sin_addr.s_addr = inet_addr(pServerAddr.c_str());
	serverListener.sin_port = htons(pPort);

	// Setup the TCP listening socket
	int ret = bind(mServerSocket, (SOCKADDR *)&serverListener, sizeof(serverListener));
	if (ret == SOCKET_ERROR) {
		std::string errorMsg = "bind failed with error: ";
		errorMsg.append(std::to_string(WSAGetLastError()));
		closesocket(mServerSocket);
		WSACleanup();
		throw std::runtime_error(errorMsg);
	}

	std::thread listeningThread( &ServerImpl::listening, this);

	listeningThread.join();
}

void ServerImpl::listening()
{
	int iResult = listen(mServerSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		std::string errorMsg = "listen failed with error: ";
		errorMsg.append(std::to_string(WSAGetLastError()));
		closesocket(mServerSocket);
		WSACleanup();
		throw std::runtime_error(errorMsg);
	}

	// Accept a client socket
	mClientSocket = accept(mServerSocket, NULL, NULL);
	if (mClientSocket == INVALID_SOCKET) {
		std::string errorMsg = "accept failed with error: ";
		errorMsg.append(std::to_string(WSAGetLastError()));
		closesocket(mServerSocket);
		WSACleanup();
		throw std::runtime_error(errorMsg);
	}

	std::thread receiverThread( &ServerImpl::receive, this);
}

void ServerImpl::receive()
{
	std::string recBuffer;

	while (mFutureObj.wait_for(std::chrono::milliseconds(10)) == std::future_status::timeout)
	{
		const static unsigned bufferSize = 1024;
		// receive messages
		char buffer[bufferSize];

		int bytesReceived = recv(mClientSocket, buffer, bufferSize, 0);

		// Append to the buffer and process it on otherplace
		if (bytesReceived > 0)
		{
			recBuffer.append(buffer);
		}
		else if (bytesReceived == 0)
		{
			std::cout << "Connection closed..." << std::endl;
		}
		else
		{
			std::cout << "Receive error: " << WSAGetLastError();
		}

		DataEncoderImpl::processIncomingMessage(recBuffer);

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

int ServerImpl::sendMessage(std::string pMessage)
{
	int sendResult = send(mClientSocket, pMessage.c_str(), pMessage.size(), 0);
	if (sendResult == SOCKET_ERROR) 
	{
		std::cout << "send failed with error: " << WSAGetLastError() << std::endl;
		closesocket(mClientSocket);
		WSACleanup();
	}
	return 0;
}

void ServerImpl::closeConnetions()
{
	int closeResult = shutdown(mClientSocket, SD_BOTH);
	if (closeResult == SOCKET_ERROR)
	{
		std::cout << "Client socket shutdown failed with error: " << WSAGetLastError() << std::endl;
	}

	closesocket(mClientSocket);
	WSACleanup();

	closesocket(mServerSocket);
	WSACleanup();

}
