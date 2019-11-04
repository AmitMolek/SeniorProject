#include "BaseClient.h"

#include "SocketHelper.h"

#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

BaseClient::BaseClient(PCSTR _serverAddress, PCSTR _dataPort, PCSTR _instructionPort) {
	dataResult = nullptr;
	instructionResult = nullptr;

	dataSocket = INVALID_SOCKET;
	instructionSocket = INVALID_SOCKET;

	serverAddress = _serverAddress;
	dataPort = _dataPort;
	instructionPort = _instructionPort;

	GetInstructionAddressInfo();
	GetInstructionSocket();
	ConnectInstructionSocket();
}

void InitHints(struct addrinfo& hints){
	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
}

void BaseClient::GetDataAddressInfo() {
	struct addrinfo hints;

	InitHints(hints);

	dataResult = SocketHelper::GetAddressInfo(serverAddress, dataPort, hints);
}

void BaseClient::GetInstructionAddressInfo() {
	struct addrinfo hints;

	InitHints(hints);

	instructionResult = SocketHelper::GetAddressInfo(serverAddress, instructionPort, hints);
}

void BaseClient::GetDataSocket() {
	SocketHelper::GetSocket(dataResult, dataSocket);
}

void BaseClient::GetInstructionSocket() {
	SocketHelper::GetSocket(instructionResult, instructionSocket);
}

void BaseClient::ConnectInstructionSocket() {
	int returnCode;

	returnCode = connect(instructionSocket, instructionResult->ai_addr, instructionResult->ai_addrlen);

	if (returnCode == SOCKET_ERROR) {
		closesocket(instructionSocket);
		instructionSocket = INVALID_SOCKET;
	}

	freeaddrinfo(instructionResult);

	if (instructionSocket == INVALID_SOCKET) {
		cerr << "Failed to connect to server\n";
		WSACleanup();
		exit(EXIT_FAILURE);
	}
}