#include "DataEncoderImpl.h"
#include <iostream>

std::unique_ptr<std::string> DataEncoderImpl::encapsulateData(const std::string& pMessage)
{
    std::string encodedMessage = pMessage;
    // TODO
    return std::make_unique<std::string>(encodedMessage);
}

std::unique_ptr<std::string> DataEncoderImpl::decapsulateData(const std::string& pMessage)
{
    std::string decodedMessage = pMessage;
    // TODO
    return std::make_unique<std::string>(decodedMessage);
}

void DataEncoderImpl::processIncomingMessage(std::string& pMessage)
{
    std::cout << "Decoded incoming message: " 
        << std::endl 
        << decapsulateData(pMessage)
        << std::endl;

    pMessage.clear();
}
