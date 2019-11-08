#include "BaseServer.h"
#include "SocketHelper.h"
#include "ConsoleOutput.h"

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
#define DATA_SOCKET_TIMEOUT 5

void Thread_HandleClientInstructions(SOCKET&& clientSocket, SOCKET&& dataSocket, string clientAddr);
void* get_in_addr(struct sockaddr* sa);

void Thread_Accept(SOCKET _socket, string socketPrefix, bool spawnThread);

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

	//StartAccepting();
	std::thread instructionAcceptThread (Thread_Accept, instructionSocket, "instruction", true);
	std::thread dataAcceptThread(Thread_Accept, dataSocket, "data", false);
	instructionAcceptThread.join();
	dataAcceptThread.join();
}

void BaseServer::StartAccepting() {
	SOCKET clientInstructionSocket = INVALID_SOCKET;
	SOCKET clientDataSocket = INVALID_SOCKET;
	struct sockaddr_storage clientAddrInstruction, clientAddrData;
	socklen_t clientInstructionAddrSize = sizeof(clientAddrInstruction);
	socklen_t clientDataAddrSize = sizeof(clientAddrData);

	char clientAddress[INET6_ADDRSTRLEN];
	string sendMsg;

	cout << "[INFO] Started accepting clients\n";

	while (true){
		clientInstructionSocket = INVALID_SOCKET;
		clientDataSocket = INVALID_SOCKET;

		clientInstructionSocket = 
			accept(instructionSocket, (struct sockaddr*) &clientAddrInstruction, &clientInstructionAddrSize);

		// Checking if the instruction socket is valid
		if (clientInstructionSocket == INVALID_SOCKET) {
			cerr << "[ERROR] Failed to connect to client\n";
			continue;
		}
		cout << "Instruction Socket = " << clientInstructionSocket << "\n";
		sendMsg = "pass:instruction_socket:" + std::to_string(clientInstructionSocket);
		send(clientInstructionSocket, sendMsg.c_str(), sendMsg.size(), 0);

		// Getting the client address so we can print it
		inet_ntop(
			clientAddrInstruction.ss_family,
			get_in_addr((struct sockaddr*) & clientAddrInstruction),
			clientAddress,
			sizeof(clientAddress));

		cout << "[INFO] Waiting for data socket connection from " << clientAddress << "\n";

		// Lambda for checking for connection to the data socket
		auto checkForDataConnection = [](SOCKET _dataSocket, struct sockaddr_storage clientAddr, socklen_t clientAddrSize, string clientStrAddress, bool* cts) {
			std::chrono::milliseconds sleepFor(50);

			// Setting the _dataSocket to be unblocking so we can poll and check for cancellation
			u_long socketUnblockMode = 1;
			ioctlsocket(_dataSocket, FIONBIO, &socketUnblockMode);

			SOCKET clientDataSocket = INVALID_SOCKET;
			do {
				// Just to slow down the polling
				std::this_thread::sleep_for(sleepFor);

				// Checking if this operation is being cancelled
				if (*cts) {
					cout << "[INFO] Stopped waiting for data socket from " << clientStrAddress << "\n";
					break;
				}
				cout << "[LOOK HERE] before: " << clientDataSocket << "\n";
				clientDataSocket = accept(_dataSocket, (struct sockaddr*) & clientAddr, &clientAddrSize);
				cout << "[LOOK HERE] after: " << clientDataSocket << "\n";
			} while (clientDataSocket == INVALID_SOCKET);
			ConsoleOutput() << "[BITCH] Background socket listening ended\n";
			return clientDataSocket;
		};

		// cts = CancellationTokenSource, used to cancel the async operation
		bool cts = false;
		// Starting async operationfor connecting the data socket of this client
		auto acceptDataSocket = std::async(launch::async, checkForDataConnection, dataSocket, clientAddrData, clientDataAddrSize, clientAddress, &cts);

		// if acceptDataSocket is taking to long we cancel the operation
		std::chrono::seconds dataSocketTimeoutSpan(DATA_SOCKET_TIMEOUT);
		while (acceptDataSocket.wait_for(dataSocketTimeoutSpan) == std::future_status::timeout) {
			cout << "TIMEOUT BITCH\n";
			cts = true;
		}

		if (cts) {
			sendMsg = "fail:data_socket_timeout";
			send(clientInstructionSocket, sendMsg.c_str(), sendMsg.size(), 0);
			continue;
		}

		try {
			clientDataSocket = acceptDataSocket.get();
			if (clientDataSocket == INVALID_SOCKET) {
				sendMsg = "fail:data_socket_invalid";
				send(clientInstructionSocket, sendMsg.c_str(), sendMsg.size(), 0);
			} else {
				std::thread clientThread(
					Thread_HandleClientInstructions,
					clientInstructionSocket,
					clientDataSocket,
					clientAddress);
				clientThread.detach();
			}
		} catch (std::exception& e) {
			sendMsg = "fail:data_socket_accept";
			send(clientInstructionSocket, sendMsg.c_str(), sendMsg.size(), 0);
		}

		cts = true;
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
	string clientAddress){

	string msgToClient;
	char msgBuffer[BUFFER_SIZE];

	int recievedBytes = -1;

	msgToClient = "pass:connected";
	send(instructionSocket, msgToClient.c_str(), msgToClient.size(), 0);

	cout << "[INFO] Client connected [" << clientAddress << "]\n";

	// Waiting for instructions
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

	ConsoleOutput() << "[INFO] Client disconnected [" << clientAddress << "]\n";
	//cout << "[INFO] Client disconnected [" << clientAddress << "]\n";
}

void Thread_HandleClient(SOCKET instructionSocket, string clientAddress){
	char msgBuffer[BUFFER_SIZE];

	int recievedBytes = -1;

	SOCKET dataSocket = INVALID_SOCKET;

	string clientPrefix = "[" + clientAddress + "]";
	string msgToSend;

	ConsoleOutput() << "[INFO]" << clientPrefix << " Started handling client\n";

	// Keep listrning for instructions from the client
	while ((recievedBytes = recv(instructionSocket, msgBuffer, BUFFER_SIZE, 0)) != -1){
		vector<string> splittedMsg;
		string msgBuffer(msgBuffer, msgBuffer + recievedBytes);
		ConsoleOutput() << "[INFO]" << clientPrefix << " Sent instruction: " << msgBuffer << "\n";

		boost::split(splittedMsg, msgBuffer, boost::is_any_of("|"));
		for(string inst : splittedMsg){
			vector<string> splittedInstructions;
			boost::split(splittedInstructions, inst, boost::is_any_of(":"));
			if (splittedInstructions.size() > 0) {
				if (splittedInstructions[0] == "pass") {
					if (splittedInstructions[1] == "data_socket_id") {
						// pass:data_socket_id:FD -> Client sending the server the data socket FD (File Descriptor)
						dataSocket = stoi(splittedInstructions[2]);
						ConsoleOutput() << "[INFO]" << clientPrefix << " Got data socket " << dataSocket << "\n";

						msgToSend = "|pass:socket_id";
						send(instructionSocket, msgToSend.c_str(), msgToSend.size(), 0);
					}
				}
			}
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

		sendMsg = "|pass:" + socketPrefix + "_socket:" + std::to_string(clientSocket);
		send(clientSocket, sendMsg.c_str(), sendMsg.size(), 0);

		if (spawnThread) {
			std::thread clientThread(Thread_HandleClient, clientSocket, clientAddress);
			clientThread.detach();
		}
	}
}