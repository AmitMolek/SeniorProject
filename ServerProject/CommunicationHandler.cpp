#include "CommunicationHandler.h"
#include "FileUploadInfo.h"

#include <iostream>
#include <WinSock2.h>
#include <vector>
#include <string>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

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
	char buffer[BUFFER_SIZE];
	int receivedBytes = -1;

	while ((receivedBytes = recv(socket, buffer, BUFFER_SIZE, 0)) != -1) {
		std::string receivedMsg(buffer, buffer + receivedBytes);
		std::vector<std::string> splitMsg;
		boost::split(splitMsg, receivedBytes, boost::is_any_of("BOF"));

	}
}