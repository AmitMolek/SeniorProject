#include "BaseServer.h"
#include "SocketHelper.h"

#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>
#include <vector>
#include <future>
#include <chrono>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

using namespace std;

#define BUFFER_SIZE 1024

void Thread_HandleClientInstructions(SOCKET&& clientSocket, struct sockaddr_storage clientAddr);

extern bool SendMsg(SOCKET& socket, string msg, const int bufferSize);

BaseServer::BaseServer(PCSTR _dataPort, PCSTR _instructionPort) {
	instructionSocket = INVALID_SOCKET;

	instructionResult = nullptr;

	dataPort = _dataPort;
	instructionPort = _instructionPort;

	StartServer();
}

void BaseServer::StartServer() {
	CreateInstructionSocket();
	BindInstructionPort();
	ListenInstructionSocket();

	StartAccepting();
}

bool test(int x){}

void BaseServer::StartAccepting() {
	SOCKET clientInstructionSocket = INVALID_SOCKET;
	struct sockaddr_storage clientAddr;
	socklen_t clientAddrSize = sizeof(clientAddr);

	cout << "[INFO] Started accepting clients\n";

	while (true){
		clientInstructionSocket = accept(instructionSocket, (struct sockaddr*) &clientAddr, &clientAddrSize);

		//std::async(launch::async, Socket, clientInstructionSocket, "connect:data_socket", BUFFER_SIZE);
		if (clientInstructionSocket == INVALID_SOCKET) {
			cerr << "Failed to connect to client\n";
			continue;
		}

		std::thread clientThread(Thread_HandleClientInstructions, clientInstructionSocket, clientAddr);
		clientThread.detach();
	}

}

void BaseServer::CreateInstructionSocket() {
	struct addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	instructionResult = SocketHelper::GetAddressInfo(NULL, instructionPort, hints);

	SocketHelper::GetSocket(instructionResult, instructionSocket);
	cout << "[INFO] Created instruction socket\n";
}

void BaseServer::BindInstructionPort() {
	BindPort(instructionSocket, instructionResult);
	cout << "[INFO] Bound instruction socket\n";
}

void BaseServer::ListenInstructionSocket() {
	ListenToSocket(instructionSocket);
	cout << "[INFO] Started listening to instruction socket\n";
}

void BaseServer::ListenToSocket(const SOCKET& socket){
	int returnCode;

	returnCode = listen(socket, SOMAXCONN);

	if (returnCode == SOCKET_ERROR) {
		cerr << "Failed to listen to socket with error: " << WSAGetLastError() << "\n";
		closesocket(socket);
		WSACleanup();
		exit(EXIT_FAILURE);
	}
}

void BaseServer::BindPort(SOCKET& socket, struct addrinfo* result) {
	int returnCode;

	returnCode = ::bind(socket, result->ai_addr, (int)result->ai_addrlen);

	if (returnCode == SOCKET_ERROR) {
		cerr << "Failed to bind socket with error: " << WSAGetLastError() << "\n";
		closesocket(socket);
		WSACleanup();
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(result);
}

void* get_in_addr(struct sockaddr* sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void Thread_HandleClientInstructions(SOCKET&& clientSocket, struct sockaddr_storage clientAddr){
	char clientAddress[INET6_ADDRSTRLEN];
	char msgBuffer[BUFFER_SIZE];

	int recievedBytes = -1;

	inet_ntop(clientAddr.ss_family, get_in_addr((struct sockaddr*)&clientAddr), clientAddress, sizeof(clientAddress));

	cout << "[INFO] Client connected [" << clientAddress << "]\n";

	while ((recievedBytes = recv(clientSocket, msgBuffer, BUFFER_SIZE, 0)) != -1){
		string msgBuffer;
		vector<string> splittedMsg;

		// Checks if the client closed the connection
		if (recievedBytes == 0) {
			cout << "[INFO] Client disconnected [" << clientAddress << "]\n";
			break;
		}

		// Splits the message to it's components
		boost::split(splittedMsg, msgBuffer, boost::is_any_of(","));

		if (splittedMsg.size() > 0) {
			if (splittedMsg[0] == "put") {
				cout << "[FILE] Client waits for data socket\n";
			}
		}
	}

	cout << "[INFO] Client disconnected [" << clientAddress << "]\n";
}