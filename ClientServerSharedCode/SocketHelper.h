#pragma once

#include <iostream>
#include <WinSock2.h>
#include <string>

class SocketHelper {
public:
	static struct addrinfo* GetAddressInfo(PCSTR serverAddress, PCSTR connectionPort, struct addrinfo hints);
	static void GetSocket(struct addrinfo* result, SOCKET& outputSocket);
	static bool SendMsg(SOCKET socket, std::string msg, const int bufferSize);
	static void InitWinSock();

};

