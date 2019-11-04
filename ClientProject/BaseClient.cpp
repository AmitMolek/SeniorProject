#include "BaseClient.h"

#include "SocketHelper.h"

#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <future>
#include <chrono>

using namespace std;

#define INSTRUCTION_BUFFER_SIZE 1024

void Thread_HandleInstruction(SOCKET&& instructionSocket);

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

	GetDataAddressInfo();
	GetDataSocket();

	ConnectInstructionSocket();
	Sleep(5000);
	ConnectDataSocket();

	while (true);
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

void BaseClient::ConnectDataSocket() {
	int returnCode;

	std::chrono::seconds span(3);
	auto dataSocketConnect = std::async(
		launch::async, [](SOCKET* _dataSocket, struct addrinfo* dataResult) {
			return connect(*_dataSocket, dataResult->ai_addr, dataResult->ai_addrlen);
		}
	, &dataSocket, dataResult);

	//while (dataSocketConnect.wait_for(span) != std::future_status::timeout){
	//	
	//}

	returnCode = dataSocketConnect.get();
	//returnCode = connect(dataSocket, dataResult->ai_addr, dataResult->ai_addrlen);

	if (returnCode == SOCKET_ERROR) {
		closesocket(dataSocket);
		dataSocket = INVALID_SOCKET;
	}

	freeaddrinfo(dataResult);

	if (dataSocket == INVALID_SOCKET) {
		cerr << "Failed to connect to server\n";
		WSACleanup();
		exit(EXIT_FAILURE);
	}
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

	std::thread instructionThread(
		Thread_HandleInstruction,
		instructionSocket);
	instructionThread.detach();
}

void Thread_HandleInstruction(SOCKET&& instructionSocket) {
	int receivedBytes = -1; 
	char instructionBuffer[INSTRUCTION_BUFFER_SIZE];

	while ((receivedBytes = recv(instructionSocket, instructionBuffer, INSTRUCTION_BUFFER_SIZE, 0)) != -1){
		cout << instructionBuffer << "\n";
		string receivedMsg(instructionBuffer);

		if (receivedBytes == 0 || receivedMsg == "timeout:data_socket") {
			cout << "Disconnected\n";
			shutdown(instructionSocket, SD_SEND);
			closesocket(instructionSocket);
			break;
		}
	}

	cout << "Connection disconnected\n";
}