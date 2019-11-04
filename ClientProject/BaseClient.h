#pragma once

#include <WinSock2.h>

class BaseClient {
public:
	SOCKET dataSocket;
	SOCKET instructionSocket;

	PCSTR serverAddress;
	PCSTR dataPort;
	PCSTR instructionPort;

	struct addrinfo* dataResult;
	struct addrinfo* instructionResult;

	BaseClient(PCSTR _serverAddress, PCSTR _dataPort, PCSTR _instructionPort);

	void GetDataAddressInfo();
	void GetInstructionAddressInfo();
	void GetDataSocket();
	void GetInstructionSocket();
	void ConnectInstructionSocket();
};

