#pragma once

#include "VStorage.h"

#include <iostream>
#include <WinSock2.h>

class ConnectionInfo {
public:
	SOCKET* instructionSocket;
	SOCKET* dataSocket;
	std::string clientAddress;
	VStorage* storage;
	std::string username;

	ConnectionInfo() = delete;
	ConnectionInfo(SOCKET* _instructionSocket, 
				   SOCKET* _dataSocket, 
				   std::string _clientAddress,
				   VStorage* _storage);

};

