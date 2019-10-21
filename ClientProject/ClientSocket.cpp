#include "ClientSocket.h"

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

namespace fs = filesystem;

ClientSocket::ClientSocket() {
	BasicSocket::GetAddressInfo();
	BasicSocket::CreateConnectionSocket();
	ConnectSocket();
	Send();
}

ClientSocket::ClientSocket(PCSTR _clientAddress) : BasicSocket(_clientAddress, "23456"){
	BasicSocket::GetAddressInfo();
	BasicSocket::CreateConnectionSocket();
	ConnectSocket();
	Send();
}

void ClientSocket::FillHints() {
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
}

void ClientSocket::ConnectSocket() {
	int returnCode;

	returnCode = connect(listenSocket, result->ai_addr, (int)result->ai_addrlen);

	if (returnCode == SOCKET_ERROR) {
		closesocket(listenSocket);
		listenSocket = INVALID_SOCKET;
	}

	freeaddrinfo(result);

	if (listenSocket == INVALID_SOCKET) {
		cout << "[ERROR] Failed to connect to server" << endl;
		WSACleanup();
		exit(EXIT_FAILURE);
	}
}

void ClientSocket::Send() {
	char msg[1024 + 1];
	string consoleMsg;

	fs::path projectPath = fs::current_path();
	fs::path testFolder = "TestFiles";

	fs::path testPath = projectPath;
	fs::path testFilePath;
	testPath /= testFolder;
	testFilePath = testPath;
	testFilePath /= "testFile.jpg";

	ifstream testFile(testFilePath, std::ios::binary);

	const int bufferSize = 1024;
	char buffer[bufferSize];

	while (true){
		cout << "Enter to send file's header\n";
		std::getline(cin, consoleMsg);

		streampos begTell = testFile.tellg();
		testFile.seekg(0, std::ios::end);
		streampos endTell = testFile.tellg();
		streampos fileSizeStream = endTell - begTell;

		ostringstream posStream;
		posStream << fileSizeStream;

		consoleMsg = "FILE=" + posStream.str();
		consoleMsg += ",";
		consoleMsg += testFilePath.extension().string();

		if (send(listenSocket, consoleMsg.c_str(), 1024, 0) == -1) {
			cerr << "Failed to send file header\n";
		}

		cout << "Enter to send file\n";
		std::getline(cin, consoleMsg);

		int i = 0;
		testFile.seekg(0, std::ios::beg);
		while (!testFile.eof()) {
			testFile.read(buffer, bufferSize);

			if (i == 250) {
				consoleMsg = "FILE_CANCELED";

				if (send(listenSocket, consoleMsg.c_str(), 1024, 0) == -1) {
					cerr << "Failed to send msg\n";
				}
			}

			if (send(listenSocket, buffer, bufferSize, 0) == -1) {
				cerr << "Failed to send file\n";
			}
			i++;
		}

		consoleMsg = "FILE_END";

		if (send(listenSocket, consoleMsg.c_str(), 1024, 0) == -1) {
			cerr << "Failed to send msg\n";
		}
	}
	cin >> msg;

}

void ClientSocket::Disconnect() {
	int returnCode;

	returnCode = shutdown(listenSocket, SD_SEND);

	if (returnCode == SOCKET_ERROR) {
		cout << "[ERROR] Client shutdown failed with error: " << WSAGetLastError() << endl;
		closesocket(listenSocket);
		WSACleanup();
		exit(EXIT_FAILURE);
	}

	cout << "[INFO] Disconnected the client" << endl;
}