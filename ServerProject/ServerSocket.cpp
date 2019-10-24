/*
TODO:
	- Create two sockets for each client that connects (one for instruction, one for data transfer)
	- Test the two sockets configuration
	- Code the instruction handler function so it listens for instructions
	  and creates a new thread for the file transfer
*/

#include "ServerSocket.h"

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>
#include <filesystem>
#include <sstream>
#include <fstream>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <atomic>

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

class FileTransferFlags {
public:
	atomic_bool cancellationToken;
	atomic_bool isTransferCanceled;
	atomic_bool isTransferFinished;

	FileTransferFlags() {
		cancellationToken = false;
		isTransferCanceled = false;
		isTransferFinished = false;
	}

};

class FileTransferInfo {
public:
	atomic_int fileSize;
	atomic<string> fileExtension;

	FileTransferInfo(int _fileSize, string _fileExtension) {
		fileSize = _fileSize;
		fileExtension = _fileExtension;
	}
};

class PathData {
public:
	fs::path projectPath;
	fs::path storageFolderPath;

	PathData() {
		projectPath = fs::current_path();

		storageFolderPath = projectPath;
		storageFolderPath /= "Storage";

		fs::create_directories(storageFolderPath);
	}
};

void ClientInstructionHandler(SOCKET& instructionSocket, SOCKET& dataSocket, struct sockaddr_storage conInfo){
	// Handle the instructions sent from the client here
	std::thread::id threadId = std::this_thread::get_id();
	char clientAddress[INET6_ADDRSTRLEN];

	const int bufferSize = 1024;
	char msgBuffer[bufferSize + 1];
	int receivedBytes = 0;

	ostringstream threadIdStream;
	threadIdStream << std::this_thread::get_id();
	string threadIdStr = threadIdStream.str();
	string threadPrefix = "[THREAD " + threadIdStr + "] ";

	inet_ntop(conInfo.ss_family, get_in_addr((struct sockaddr*) & conInfo), clientAddress, sizeof(clientAddress));

	cout << threadPrefix << "Started listening for instructions from " << clientAddress << "\n";

	while ((receivedBytes = recv(instructionSocket, msgBuffer, bufferSize, 0)) != -1){
		cout << "Echoing msg from client: " << msgBuffer << "\n";
	}
}

void FileUpload_Test(SOCKET& dataSocket, const FileTransferInfo& fileInfo, const FileTransferFlags& flags){
	const int bufferSize = 1024;
	char msgBuffer[bufferSize];
	int receivedBytes = -1;

	ostringstream threadIdStream;
	threadIdStream << std::this_thread::get_id();
	string threadIdStr = threadIdStream.str();
	string threadPrefix = "[THREAD " + threadIdStr + " |FILE] ";

	PathData paths;
	fs::path threadStorageFolder = paths.storageFolderPath;
	threadStorageFolder /= threadIdStr;
	fs::create_directories(threadStorageFolder);

	fs::path outputFilePath = threadStorageFolder;
	string fileNamePath = "tmp_" + threadIdStr + fileInfo.fileExtension.load();
	outputFilePath /= fileNamePath;

	ofstream outputFile(outputFilePath, std::ios::binary);

	while ((receivedBytes = recv(dataSocket, msgBuffer, bufferSize, 0)) != -1) {
		if (flags.cancellationToken) {
			outputFile.close();
			fs::remove(outputFilePath);
			cout << threadPrefix << "File transfer " << fileNamePath << " canceled\n";
			break;
		}

		// Handle the received bytes here (create a file, write to it and that shit...)
		outputFile.write(msgBuffer, bufferSize);
		memset(msgBuffer, 0, sizeof(char) * bufferSize);

		if (flags.isTransferCanceled || flags.isTransferFinished) {
			outputFile.close();
			cout << threadPrefix << "Closed file " << fileNamePath << "\n";
			if (flags.isTransferCanceled) {
				fs::remove(outputFilePath);
				cout << threadPrefix << "Deleted file " << fileNamePath << "\n";
			}
		}
	}
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

	while ((receivedBytes = recv(clientSocket, buffer, bufferSize, 0)) != -1){
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

void ServerSocket::AcceptTwoSocketsConnections() {
	SOCKET clientInstructionSocket = INVALID_SOCKET;
	SOCKET clientDataSocket = INVALID_SOCKET;
	
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