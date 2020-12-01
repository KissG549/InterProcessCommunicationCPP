#pragma once
#include <mutex>

class IIPCAppBase
{
public:
	virtual void run(std::string& pServerAddr, int pPort) = 0;

};
