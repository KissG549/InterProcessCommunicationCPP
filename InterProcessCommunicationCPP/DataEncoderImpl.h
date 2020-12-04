#pragma once
#include <string>
#include <memory>

class DataEncoderImpl
{
public:
	DataEncoderImpl() = delete;
	
	static std::unique_ptr<std::string> encapsulateData(const std::string& pMessage);
	static std::unique_ptr<std::string> decapsulateData(const std::string& pMessage);

	static void processIncomingMessage(std::string& pMessage);

};