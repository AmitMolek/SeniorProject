#include "CommunicationHandler.h"
#include "FileUploadInfo.h"

#include <iostream>
#include <WinSock2.h>
#include <vector>
#include <string>

constexpr int BUFFER_SIZE = 1024;

std::pair<int, std::string> CommunicationHandler::ReceiveMsg(SOCKET& socket) {
	int receivedBytes = -1;
	char buffer[BUFFER_SIZE];

	receivedBytes = recv(socket, buffer, BUFFER_SIZE, 0);

	if (receivedBytes == -1)
		return std::make_pair(receivedBytes, "");

	return std::make_pair(receivedBytes, std::string(buffer, buffer + receivedBytes));
}

int CommunicationHandler::SendBasicMsg(SOCKET& socket, std::string _msg){
	int sentBytes = -1;

	sentBytes = send(socket, _msg.c_str(), _msg.size(), 0);

	return sentBytes;
}

void CommunicationHandler::ReceiveFile(SOCKET& socket, std::ofstream& fileStream, FileUploadInfo& fileInfo) {
	
}