#include "SocketHelper.h"

#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

#pragma comment(lib, "Ws2_32.lib")

struct addrinfo* SocketHelper::GetAddressInfo(PCSTR serverAddress, PCSTR connectionPort, 
	struct addrinfo hints) {
	int returnCode = -1;

	struct addrinfo* result;

	if ((returnCode = getaddrinfo(serverAddress, connectionPort, &hints, &result)) != 0) {
		cerr << "Failed to get address information with code: " << gai_strerror(returnCode) << "\n";
		WSACleanup();
		exit(EXIT_FAILURE);
	}

	if (serverAddress != nullptr)
		cout << "[INFO] Got address information for " << serverAddress << ":" << connectionPort << "\n";
	return result;
}

void SocketHelper::GetSocket(struct addrinfo* result, SOCKET& outputSocket) {
	outputSocket = INVALID_SOCKET;

	if ((outputSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol)) == INVALID_SOCKET) {
		cerr << "Failed creating connection socket with code: " << WSAGetLastError() << "\n";
		freeaddrinfo(result);
		WSACleanup();
		exit(EXIT_FAILURE);
	}
}

bool SocketHelper::SendMsg(SOCKET socket, string msg, const int bufferSize) {
	//int receivedBytes = -1;
	//char buffer[1024];
	//
	//if ((receivedBytes = recv(socket, buffer, bufferSize, 0)) != -1)
	//	return false;
	//
	//return true;
	while (true);
}

void SocketHelper::InitWinSock() {
	WSADATA wsaData;
	int returnCode;

	returnCode = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (returnCode != 0) {
		cout << "[ERROR] WSAStartup failed with error: " << returnCode << endl;
		exit(EXIT_FAILURE);
	}
}