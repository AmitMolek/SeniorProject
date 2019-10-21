#pragma once

#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

class BasicSocket {
public:

	SOCKET listenSocket;

	struct addrinfo* result;
	struct addrinfo hints;
	
	PCSTR serverAddress;
	PCSTR connectionPort;

	BasicSocket();
	BasicSocket(PCSTR _serverAddress, PCSTR _connectionPort);

	virtual void FillHints() = 0;
	void GetAddressInfo();
	void CreateConnectionSocket();
	virtual void Disconnect() = 0;

	static void InitWinsock();
};

