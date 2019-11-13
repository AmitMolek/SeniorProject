#include "BaseClient.h"

#include "SocketHelper.h"

#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <future>
#include <chrono>
#include <string>
#include <fstream>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

#define INSTRUCTION_BUFFER_SIZE 1024

void Thread_HandleInstruction(SOCKET&& instructionSocket, SOCKET&& dataSocket, struct addrinfo* dataResult);

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
	//Sleep(3000);
	//ConnectDataSocket();

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
	cout << "Connected to server\n";
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

	cout << "Instruction Socket = " << instructionSocket << "\n";

	std::thread instructionThread(
		Thread_HandleInstruction,
		instructionSocket,
		dataSocket,
		dataResult);
	instructionThread.detach();
}

void testReadFile(SOCKET& insSocket, SOCKET& dataSocket) {
	fs::path projectPath = fs::current_path();
	fs::path testFolder = "TestFiles";

	fs::path testPath = projectPath;
	fs::path testFilePath;
	testPath /= testFolder;
	testFilePath = testPath;
	testFilePath /= "testFile.jpg";

	cout << "Test file path: " << testFilePath << '\n';

	ifstream testFile(testFilePath, std::ios::binary);

	if (!testFile) {
		cerr << "Test file could not be open!" << '\n';
		exit(EXIT_FAILURE);
	}

	cout << "Test file is open!" << '\n';

	const int bufferSize = 1024;
	char buffer[bufferSize];

	streampos begSeek = testFile.tellg();
	testFile.seekg(0, std::ios::end);
	streampos endSeek = testFile.tellg();
	streampos fileSize = endSeek - begSeek;

	cout << "File size = " << fileSize << '\n';
	cout << "Loops = " << fileSize / 1024 << '\n';

	testFile.seekg(0, std::ios::beg);

	fs::path outputFilePath = testPath;
	outputFilePath /= "output.jpg";

	ofstream outputFile(outputFilePath, std::ios::binary);
	if (!outputFile) {
		cerr << "Output file could not be open!" << '\n';
		exit(EXIT_FAILURE);
	}

	string msgg("|pass:file_start");
	send(dataSocket, msgg.c_str(), msgg.size(), 0);
	while (!testFile.eof()) {
		testFile.read(buffer, bufferSize);
		//outputFile.write(buffer, bufferSize);
		send(dataSocket, buffer, bufferSize, 0);
	}
	msgg = ("|pass:file_end");
	send(dataSocket, msgg.c_str(), msgg.size(), 0);

	testFile.close();
	outputFile.close();
}

void Thread_HandleInstruction(SOCKET&& instructionSocket, SOCKET&& dataSocket, struct addrinfo* dataResult) {
	int receivedBytes = -1;
	char instructionBuffer[INSTRUCTION_BUFFER_SIZE];

	string instructionSocketFd;
	string dataSocketFd;

	while ((receivedBytes = recv(instructionSocket, instructionBuffer, INSTRUCTION_BUFFER_SIZE, 0)) != -1) {
		string receivedMsg(instructionBuffer, instructionBuffer + receivedBytes);
		cout << "Msg: " << receivedMsg << "\n";

		vector<string> splitMsg;

		boost::split(splitMsg, receivedMsg, boost::is_any_of("|"));
		for(string inst : splitMsg){
			vector<string> splitInstructions;
			boost::split(splitInstructions, inst, boost::is_any_of(":"));
			if (splitInstructions.size() > 0) {
				if (splitInstructions[0] == "pass") {
					if (splitInstructions[1] == "instruction_socket") {
						instructionSocketFd = splitInstructions[2];

						//Sleep(3000);
						connect(dataSocket, dataResult->ai_addr, dataResult->ai_addrlen);

						bool readDataSocketId = false;
						while ((receivedBytes = recv(dataSocket, instructionBuffer, INSTRUCTION_BUFFER_SIZE, 0)) != -1) {
							string dataMsg(instructionBuffer, instructionBuffer + receivedBytes);
							boost::split(splitInstructions, dataMsg, boost::is_any_of(":"));
							cout << "DATA MSG " << dataMsg << "\n";
							if (splitInstructions[1] == "data_socket") {
								dataSocketFd = splitInstructions[2];

								cout << "Instruction Socket = " << instructionSocketFd << "\n";
								cout << "Data Socket = " << dataSocketFd << "\n";

								string dataSocketIdMsg = "|pass:data_socket_id:" + dataSocketFd;
								send(instructionSocket, dataSocketIdMsg.c_str(), dataSocketIdMsg.size(), 0);

								break;
							}
						}
					}else if (splitInstructions[1] == "socket_id"){
						Sleep(2000);
						cout << "SENT GAY GSHIIT\n";
						//string dataSocketIdMsg = "|pass:dick1:54,67,12,gay|pass:dick2";
						string dataSocketIdMsg = "|pass:file_send:gay.shit,400";
						send(instructionSocket, dataSocketIdMsg.c_str(), dataSocketIdMsg.size(), 0);
					}else if (splitInstructions[1] == "server_wait_on_file"){
						testReadFile(instructionSocket, dataSocket);
					}
				}
			}
		}

	}

	cout << "Connection disconnected\n";
}

//void Thread_HandleInstruction(SOCKET&& instructionSocket, SOCKET&& dataSocket, struct addrinfo* dataResult) {
//	int receivedBytes = -1; 
//	char instructionBuffer[INSTRUCTION_BUFFER_SIZE];
//
//	while ((receivedBytes = recv(instructionSocket, instructionBuffer, INSTRUCTION_BUFFER_SIZE, 0)) != -1){
//		string receivedMsg(instructionBuffer, instructionBuffer + receivedBytes);
//		cout << "Msg: " << receivedMsg << "\n";
//		
//		if (receivedMsg == "pass:instruction_socket") {
//			Sleep(6000);
//			connect(dataSocket, dataResult->ai_addr, dataResult->ai_addrlen);
//		}else if (receivedMsg == "pass:connected") {
//			cout << "Client connected to the server\n";
//		}
//
//		if (receivedBytes == 0 || receivedMsg == "fail:data_socket") {
//			cout << "Disconnected\n";
//			shutdown(instructionSocket, SD_SEND);
//			closesocket(instructionSocket);
//			break;
//		}
//	}
//
//	cout << "Connection disconnected\n";
//}