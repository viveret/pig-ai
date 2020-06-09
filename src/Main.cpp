#include "Program.hpp"
#include "Data/Config.hpp"

#include <iostream>

#ifdef _WIN32
#include <ws2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")
#endif

using namespace PigAI;

int main()
{
	#ifdef _WIN32
	WORD versionWanted = MAKEWORD(1, 1);
	WSADATA wsaData;
	if (WSAStartup(versionWanted, &wsaData)) {
		std::cout << "Error starting WSA: " << WSAGetLastError() << std::endl;
		return -1;
	}
	#endif

	Config::Init();
    AIProgram p;
    std::cout << p.label() << "\t-\t" << p.description() << std::endl;
    
    return p.run();
}