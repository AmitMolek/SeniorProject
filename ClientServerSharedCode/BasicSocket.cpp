#include "BasicSocket.h"

#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

#define LOCALHOST_IP "10.0.0.54"
#define DEFAULT_PORT "23456"

BasicSocket::BasicSocket() : BasicSocket(LOCALHOST_IP, DEFAULT_PORT) {}

BasicSocket::BasicSocket(PCSTR _serverAddress, PCSTR _connectionPort) : hints(){
	serverAddress = _serverAddress;
	connectionPort = _connectionPort;

	listenSocket = INVALID_SOCKET;

	result = nullptr;
	//serverAddress = nullptr;
	connectionPort = DEFAULT_PORT;

	//GetAddressInfo();
	//CreateConnectionSocket();
}

void BasicSocket::GetAddressInfo() {
	int returnCode;

	FillHints();

	returnCode = getaddrinfo(serverAddress, connectionPort, &hints, &result);

	if (returnCode != 0) {
		cout << "[ERROR] Getting address info failed with error " << gai_strerror(returnCode) << endl;
		WSACleanup();
		exit(EXIT_FAILURE);
	}

	cout << "[INFO] Got address info" << endl;
}

void BasicSocket::CreateConnectionSocket() {
	listenSocket = INVALID_SOCKET;

	listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (listenSocket == INVALID_SOCKET) {
		cout << "[ERROR] Failed creating connection socket with error: " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		WSACleanup();
		exit(EXIT_FAILURE);
	}

	cout << "[INFO] Created connection socket" << endl;
}

void BasicSocket::InitWinsock() {
	WSADATA wsaData;
	int returnCode;

	returnCode = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (returnCode != 0) {
		cout << "[ERROR] WSAStartup failed with error: " << returnCode << endl;
		exit(EXIT_FAILURE);
	}
}
