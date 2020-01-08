#include "CommunicationHandler.h"
#include "FileUploadInfo.h"

#include <iostream>
#include <WinSock2.h>
#include <vector>
#include <string>

constexpr int BUFFER_SIZE = 16384 * 4;

std::pair<int, std::string> CommunicationHandler::ReceiveMsg(SOCKET& socket) {
	int receivedBytes = -1;
	//char buffer[BUFFER_SIZE];

	char* buffer = new char[BUFFER_SIZE];

	receivedBytes = recv(socket, buffer, BUFFER_SIZE, 0);

	if (receivedBytes == -1)
		return std::make_pair(receivedBytes, "");

	return std::make_pair(receivedBytes, std::string(buffer, buffer + receivedBytes));

	delete[] buffer;
}

int CommunicationHandler::SendBasicMsg(SOCKET& socket, std::string _msg){
	int sentBytes = -1;

	sentBytes = send(socket, _msg.c_str(), _msg.size(), 0);

	return sentBytes;
}

bool CommunicationHandler::SendAllMsg(SOCKET& socket, const char _msg[], size_t length) {
	size_t total = 0; // how many bytes we've sent
	size_t bytesleft = length; // how many we have left to send
	size_t n = 0;
	while (total < length) {
		n = send(socket, _msg + total, bytesleft, 0);
		if (n == -1) {
			/* print/log error details */
			break;
		}
		total += n;
		bytesleft -= n;
	}

	return n == -1 ? false : true; // return -1 on failure, 0 on success
}

bool CommunicationHandler::SendAllMsg(SOCKET& socket, std::string _msg) {
	size_t total = 0; // how many bytes we've sent
	int bytesleft = _msg.size(); // how many we have left to send
	int n = 0;
	while (total < _msg.size()) {
		n = send(socket, _msg.c_str() + total, bytesleft, 0);
		if (n == -1) {
			/* print/log error details */
			break;
		}
		total += n;
		bytesleft -= n;
	}

	return n == -1 ? false : true; // return -1 on failure, 0 on success
}

void CommunicationHandler::ReceiveFile(SOCKET& socket, VFile& fileStream) {
	
}