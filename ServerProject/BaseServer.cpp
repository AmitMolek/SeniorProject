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
#define DATA_SOCKET_TIMEOUT 15

void Thread_HandleClientInstructions(SOCKET&& clientSocket, SOCKET&& dataSocket, struct sockaddr_storage clientAddr);
void* get_in_addr(struct sockaddr* sa);

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

	CreateDataSocket();
	BindDataPort();
	ListenDataSocket();

	StartAccepting();
}

void BaseServer::StartAccepting() {
	SOCKET clientInstructionSocket = INVALID_SOCKET;
	SOCKET clientDataSocket = INVALID_SOCKET;
	struct sockaddr_storage clientAddrInstruction, clientAddrData;
	socklen_t clientInstructionAddrSize = sizeof(clientAddrInstruction);
	socklen_t clientDataAddrSize = sizeof(clientAddrData);

	char clientAddress[INET6_ADDRSTRLEN];

	bool canGetDataSocket = true;

	cout << "[INFO] Started accepting clients\n";

	while (true){
		clientInstructionSocket = 
			accept(instructionSocket, (struct sockaddr*) &clientAddrInstruction, &clientInstructionAddrSize);

		inet_ntop(
			clientAddrInstruction.ss_family,
			get_in_addr((struct sockaddr*) & clientAddrInstruction),
			clientAddress,
			sizeof(clientAddress));

		cout << "[INFO] Waiting for data socket connection from " << clientAddress << "\n";

		auto acceptDataSocket = std::async(
			launch::async, 
			[](SOCKET _dataSocket, struct sockaddr_storage clientAddr, socklen_t clientAddrSize){
			return accept(_dataSocket, (struct sockaddr*) &clientAddr, &clientAddrSize);
		}, dataSocket, clientAddrData, clientDataAddrSize);

		std::chrono::seconds timeoutSpan(DATA_SOCKET_TIMEOUT);
		while (acceptDataSocket.wait_for(timeoutSpan) == std::future_status::timeout) {
			cout << "[INFO] Data socket connection timed-out for " << clientAddress << "\n";
			char errMsg[] = "timeout:data_socket";
			send(clientInstructionSocket, errMsg, sizeof(errMsg), 0);
			closesocket(clientInstructionSocket);
			clientInstructionSocket = INVALID_SOCKET;
			canGetDataSocket = false;
			break;
		}

		if (canGetDataSocket)
			clientDataSocket = acceptDataSocket.get();

		if (clientInstructionSocket == INVALID_SOCKET) {
			cerr << "[ERROR] Failed to connect to client\n";
			continue;
		}

		char successMsg[] = "success:connection";
		send(clientInstructionSocket, successMsg, sizeof(successMsg), 0);

		std::thread clientThread(
			Thread_HandleClientInstructions,
			clientInstructionSocket,
			clientDataSocket,
			clientAddrInstruction);
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

void BaseServer::CreateDataSocket() {
	struct addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	dataResult = SocketHelper::GetAddressInfo(NULL, dataPort, hints);

	SocketHelper::GetSocket(dataResult, dataSocket);
	cout << "[INFO] Created data socket\n";
}

void BaseServer::BindInstructionPort() {
	BindPort(instructionSocket, instructionResult);
	cout << "[INFO] Bound instruction socket\n";
}

void BaseServer::BindDataPort() {
	BindPort(dataSocket, dataResult);
	cout << "[INFO] Bound data socket\n";
}

void BaseServer::ListenInstructionSocket() {
	ListenToSocket(instructionSocket);
	cout << "[INFO] Started listening to instruction socket\n";
}

void BaseServer::ListenDataSocket() {
	ListenToSocket(dataSocket);
	cout << "[INFO] Started listening to data socket\n";
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

void Thread_HandleClientInstructions(
	SOCKET&& instructionSocket, 
	SOCKET&& dataSocket,
	struct sockaddr_storage clientAddr){
	char clientAddress[INET6_ADDRSTRLEN];
	char msgBuffer[BUFFER_SIZE];

	int recievedBytes = -1;

	inet_ntop(clientAddr.ss_family, get_in_addr((struct sockaddr*)&clientAddr), clientAddress, sizeof(clientAddress));

	cout << "[INFO] Client connected [" << clientAddress << "]\n";
	while (true);
	while ((recievedBytes = recv(instructionSocket, msgBuffer, BUFFER_SIZE, 0)) != -1){
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