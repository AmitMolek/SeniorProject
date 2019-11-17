#pragma once

#include "VStorage.h"
#include <WinSock2.h>

class BaseServer {
public:
	struct addrinfo* instructionResult;
	struct addrinfo* dataResult;

	SOCKET instructionSocket;
	SOCKET dataSocket;

	PCSTR dataPort;
	PCSTR instructionPort;

	VStorage& storage;

	BaseServer(PCSTR _dataPort, PCSTR _instructionPort, VStorage& _storage);

private:
	void CreateInstructionSocket();
	void CreateDataSocket();

	void BindDataPort();
	void BindInstructionPort();

	void ListenDataSocket();
	void ListenInstructionSocket();

	void GetHints(struct addrinfo& hints);
	void ListenToSocket(const SOCKET& socket);
	void BindPort(SOCKET& socket, struct addrinfo* result);
	void StartAccepting();

	void StartServer();
};

