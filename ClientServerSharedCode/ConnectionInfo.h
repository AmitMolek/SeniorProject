#pragma once

#include <iostream>
#include <WinSock2.h>

class ConnectionInfo {
public:
	SOCKET* instructionSocket;
	SOCKET* dataSocket;
	std::string clientAddress;

	ConnectionInfo() = default;
	ConnectionInfo(SOCKET* _instructionSocket, SOCKET* _dataSocket, std::string _clientAddress);

};

