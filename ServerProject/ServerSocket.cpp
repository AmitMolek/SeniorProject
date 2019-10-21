#include "ServerSocket.h"

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>
#include <filesystem>
#include <sstream>
#include <fstream>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace fs = std::experimental::filesystem;

#pragma comment(lib, "Ws2_32.lib")

ServerSocket::ServerSocket() {
	GetAddressInfo();
	CreateConnectionSocket();
	BindSocket();
	ListenToSocket();
	AcceptConnections();
}

ServerSocket::ServerSocket(PCSTR _serverAddress, PCSTR _connectionPort) 
	: BasicSocket(_serverAddress, _connectionPort){
	GetAddressInfo();
	CreateConnectionSocket();
	BindSocket();
	ListenToSocket();
	AcceptConnections();
}

void ServerSocket::FillHints() {
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
}

void ServerSocket::BindSocket() {
	int returnCode;

	returnCode = ::bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);

	if (returnCode == SOCKET_ERROR) {
		cout << "[ERROR] Failed to bind socket with error: " << WSAGetLastError() << endl;
		closesocket(listenSocket);
		WSACleanup();
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(result);
	cout << "[INFO] Bound socket" << endl;
}

void ServerSocket::ListenToSocket() {
	int returnCode;

	returnCode = listen(listenSocket, SOMAXCONN);

	if (returnCode == SOCKET_ERROR) {
		cout << "[ERROR] Failed to listen to socket with error: " << WSAGetLastError() << endl;
		closesocket(listenSocket);
		WSACleanup();
		exit(EXIT_FAILURE);
	}

	cout << "[INFO] Listening socket started\n";
}

//void ServerSocket::AcceptSingleConnection(){
//	clientSocket = INVALID_SOCKET;
//
//	clientSocket = accept(listenSocket, NULL, NULL);
//
//	if (clientSocket == INVALID_SOCKET) {
//		cout << "[ERROR] Failed to accept client with error: " << WSAGetLastError() << endl;
//		closesocket(listenSocket);
//		WSACleanup();
//		exit(EXIT_FAILURE);
//	}
//
//	cout << "[INFO] Accepted a client connection" << endl;
//}

void* get_in_addr(struct sockaddr* sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// File receive msg: FILE=file size, file extension
// File end msg: FILE_END
void TestThreadFunc(SOCKET&& clientSocket, struct sockaddr_storage conInfo) {
	std::thread::id threadId = std::this_thread::get_id();
	char clientAddress[INET6_ADDRSTRLEN];

	const int bufferSize = 1024;
	char buffer[bufferSize + 1];
	int receivedBytes = 0;

	ostringstream threadIdStream;
	threadIdStream << std::this_thread::get_id();
	string threadIdStr = threadIdStream.str();
	string threadPrefix = "[THREAD " + threadIdStr + "] ";

	fs::path projectPath = fs::current_path();
	fs::path projectStoragePath = projectPath;
	projectStoragePath /= "File_Storage";
	fs::path storageFolder = projectStoragePath;
	storageFolder /= threadIdStr;
	fs::create_directories(storageFolder);

	inet_ntop(conInfo.ss_family, get_in_addr((struct sockaddr*) & conInfo), clientAddress, sizeof(clientAddress));

	cout << threadPrefix << "New client " << clientAddress << '\n';

	while ((receivedBytes = recv(clientSocket, buffer, bufferSize, 0) != -1)){
		string rMsg(buffer);
		vector<string> splittedMsg;
		vector<string> filePieces;

		if (receivedBytes == 0) {
			cout << threadPrefix << " lost connection" << '\n';
			break;
		}

		boost::split(splittedMsg, rMsg, boost::is_any_of("="));
		if (splittedMsg.size() > 0) {
			if (splittedMsg[0] == "FILE") {
				cout << threadPrefix << "Waiting for file\n";
				boost::split(splittedMsg, splittedMsg[1], boost::is_any_of(","));
				int fileSize = stoi(splittedMsg[0]);
				string fileExtension = splittedMsg[1];
				fs::path outputFilePath = storageFolder;
				outputFilePath /= ("tmp" + fileExtension);

				ofstream outputFile(outputFilePath, std::ios::binary);
				
				int fileReceivedBytes;
				char fileBuffer[bufferSize];
				while ((fileReceivedBytes = recv(clientSocket, fileBuffer, bufferSize, 0)) != -1){

					string bufferStr(fileBuffer);
					if (bufferStr.find("FILE_END") != std::string::npos) {
						// If the client finished sending the file
						outputFile.close();
						cout << threadPrefix << "Finished getting file\n";
						break;
					}else if (bufferStr.find("FILE_CANCELED") != std::string::npos){
						// If the client canceled the sending of the file
						outputFile.close();
						if (!fs::remove(outputFilePath)) {
							cout << threadPrefix << "Failed deleting the canceled file\n";
						}
						cout << threadPrefix << "Client canceled the sending\n";
						break;
					}

					//filePieces.push_back(string(fileBuffer));
					outputFile.write(fileBuffer, bufferSize);
					memset(fileBuffer, 0, sizeof(char) * bufferSize);
				}

			}
		}
	}

	cout << threadPrefix << "Stopped receiving from " << clientAddress << '\n';
}

void ServerSocket::AcceptConnections() {
	SOCKET newClientSocket = -1;
	struct sockaddr_storage clientAddr;
	socklen_t clientAddrSize = sizeof(clientAddr);

	cout << "[INFO] Started listening for clients\n";

	while (true){
		newClientSocket = accept(listenSocket, (struct sockaddr*) &clientAddr, &clientAddrSize);

		if (newClientSocket == INVALID_SOCKET) {
			cerr << "[ERROR] Failed to connect to client\n";
			continue;
		}
		
		std::thread receiveThread(TestThreadFunc, newClientSocket, clientAddr);
		receiveThread.detach();
	}
}

void ServerSocket::Disconnect() {

}

//void ServerSocket::Disconnect() {
//	int returnCode;
//
//	returnCode = shutdown(clientSocket, SD_SEND);
//
//	if (returnCode == SOCKET_ERROR) {
//		cout << "[ERROR] Failed to shutdown server with error: " << WSAGetLastError() << endl;
//		closesocket(clientSocket);
//		WSACleanup();
//		exit(EXIT_FAILURE);
//	}
//
//	closesocket(clientSocket);
//	WSACleanup();
//
//	cout << "[INFO] Server shutdown" << endl;
//}