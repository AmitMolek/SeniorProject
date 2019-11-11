#include "BaseServer.h"
#include "SocketHelper.h"
#include "ConsoleOutput.h"
#include "InstructionResolver.h"
#include "CommunicationHandler.h"

#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>
#include <vector>
#include <future>
#include <chrono>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <ConnectionInfo.h>
#include "InstructionHandler.h"

using namespace std;

constexpr auto BUFFER_SIZE = 1024;

void* get_in_addr(struct sockaddr* sa);
void Thread_Accept(SOCKET _socket, string socketPrefix, bool spawnThread);

BaseServer::BaseServer(PCSTR _dataPort, PCSTR _instructionPort) {
	instructionSocket = INVALID_SOCKET;
	dataSocket = INVALID_SOCKET;

	instructionResult = nullptr;
	dataResult = nullptr;

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
	std::thread instructionAcceptThread(Thread_Accept, instructionSocket, "instruction", true);
	std::thread dataAcceptThread(Thread_Accept, dataSocket, "data", false);
	instructionAcceptThread.detach();
	dataAcceptThread.detach();
}

void BaseServer::GetHints(struct addrinfo& hints){
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
}

void BaseServer::CreateInstructionSocket() {
	struct addrinfo hints;

	GetHints(hints);

	instructionResult = SocketHelper::GetAddressInfo(NULL, instructionPort, hints);

	SocketHelper::GetSocket(instructionResult, instructionSocket);
	cout << "[INFO] Created instruction socket\n";
}

void BaseServer::CreateDataSocket() {
	struct addrinfo hints;

	GetHints(hints);

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

void Thread_HandleClient(SOCKET instructionSocket, string clientAddress){
	char msgBuffer[BUFFER_SIZE];

	int recievedBytes = -1;

	SOCKET dataSocket = INVALID_SOCKET;

	string clientPrefix = "[" + clientAddress + "]";
	string msgToSend;

	ConnectionInfo clientCon(&instructionSocket, &dataSocket, clientAddress);

	ConsoleOutput() << "[INFO]" << clientPrefix << " Started handling client\n";

	pair<int, string> msgInfo;
	while ((msgInfo = CommunicationHandler::ReceiveMsg(instructionSocket)).first != -1) {
		string msg = msgInfo.second;
		ConsoleOutput() << "[INFO]" << clientPrefix << " Sent: " << msg << "\n";

		// Resolve the message for instructions
		vector<Instruction> instructions = InstructionResolver::Resolve(msg);
		for (Instruction inst : instructions) {
			ConsoleOutput() << "[INFO]" << clientPrefix << " Instruction " << inst << "\n";
			InstructionHandler::HandleInstruction(inst, clientCon);
		}
	}

	ConsoleOutput() << "[INFO] Client [" << clientAddress << "] lost connection\n";
}

void Thread_Accept(SOCKET _socket, string socketPrefix, bool spawnThread = false) {
	SOCKET clientSocket = INVALID_SOCKET;

	struct sockaddr_storage clientAddr;
	socklen_t clientAddrSize = sizeof(clientAddr);

	char clientAddress[INET6_ADDRSTRLEN];
	string sendMsg;

	ConsoleOutput() << "[INFO] Start accepting clients on the " << socketPrefix << " socket\n";

	while (true) {
		clientSocket = INVALID_SOCKET;

		clientSocket = accept(_socket, (struct sockaddr*) &clientAddr, &clientAddrSize);

		if (clientSocket == INVALID_SOCKET)
			continue;

		inet_ntop(clientAddr.ss_family, get_in_addr((struct sockaddr*) &clientAddr), clientAddress, sizeof(clientAddress));

		ConsoleOutput() << "[INFO] Accepted client [" << clientAddress << "] on the " << socketPrefix << " socket\n";

		CommunicationHandler::SendBasicMsg(clientSocket, 
										   "|pass:" + socketPrefix + "_socket:" + std::to_string(clientSocket));

		if (spawnThread) {
			std::thread clientThread(Thread_HandleClient, clientSocket, clientAddress);
			clientThread.detach();
		}
	}
}