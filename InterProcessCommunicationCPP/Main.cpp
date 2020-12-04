#include <iostream>
#include <algorithm>
#include "ClientImpl.h"
#include "ServerImpl.h"

void printUsage()
{
    std::cout << "The purpose of this program is to demonstrate data exchange between two processes using bidirectional communication.\n" \
        "Implemented data exchange method: Socket-JSON over TCP\n\n"  \
        "Usage:\n" \
        "\t -c server_host:port_number\t\tRun in client mode, connect to the specific address:port\n" \
        "[or]\n" \
        "\t -c 10.0.0.1:3001\n" \
        "\t-s listening_address port_number\t\t Run in server mode, listening on specific IP and port\n" \
        "\t\t -s 127.0.0.1:3001\n" \
        "[or]\n" \
        "\t\t -s 3001\n" \
        "\t ?\t\tDisplay this message." << std::endl;
}

/*
    Splits the pAddress to IP and PORT if there is a : separator
*/
std::pair<std::string, std::string> splitAddress(std::string& pAddress)
{
    std::string ip;
    std::string port;

    bool found = false;
    for (auto& ch : pAddress)
    {
        if (ch == ':')
        {
            found = true;
        }
        else if (!found)
        {
            ip.push_back(ch);
        }
        else
        {
            port.push_back(ch);
        }
    }

    return { ip, port };
}

int main(int pArgC, char* pArgV[])
{

    if (pArgC < 2)
    {
        std::cout << "Missing parameters! Please check and try it again!" << std::endl;
        printUsage();
        return -1;
    }

    IIPCAppBase* application = nullptr;

    std::string argAddress = pArgV[2];

    /*
        Split the parameters to IP address and port
    */
    auto addrPair = splitAddress(argAddress);

    /*
        Create the client
    */
    if ( strcmp(pArgV[1],  "-c") == 0 )
    {
        application = new ClientImpl();
        std::cout << "<Running in client mode>" << std::endl;
    }
    else
    {
    /*
        Otherwise create the server
    */
        application = new ServerImpl();
        std::cout << "<Running in server mode>" << std::endl;
    }

    try
    {
        application->run(
            addrPair.first,
            std::atoi(addrPair.second.c_str())
        );
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }

	return 0;
}