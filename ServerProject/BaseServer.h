#pragma once
#include <WinSock2.h>

class BaseServer {
public:
	struct addrinfo* instructionResult;
	struct addrinfo* dataResult;

	SOCKET instructionSocket;
	SOCKET dataSocket;

	PCSTR dataPort;
	PCSTR instructionPort;

	BaseServer(PCSTR _dataPort, PCSTR _instructionPort);

	void CreateInstructionSocket();
	void CreateDataSocket();

	void BindDataPort();
	void BindInstructionPort();

	void ListenDataSocket();
	void ListenInstructionSocket();
private:
	void ListenToSocket(const SOCKET& socket);
	void BindPort(SOCKET& socket, struct addrinfo* result);
	void StartAccepting();

	void StartServer();
};

