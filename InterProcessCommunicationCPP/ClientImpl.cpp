#include "ClientImpl.h"

ClientImpl::ClientImpl()
{
}

ClientImpl::~ClientImpl()
{
}

void ClientImpl::run(std::string& pServerAddr, int pPort)
{
}

bool ClientImpl::connect(std::string pServerAddr, int pPort)
{
    return false;
}

void ClientImpl::disconnect()
{
}

void ClientImpl::receive()
{
}

int ClientImpl::send(std::string pMessage)
{
    return 0;
}
