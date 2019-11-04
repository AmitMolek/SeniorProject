#pragma once

#include <WinSock2.h>
#include <mutex>
#include <vector>

class SocketPair {
public:
	SOCKET& instructionSocket;
	SOCKET& dataSocket;
};

class SocketVector {
public:

	std::vector<SocketPair> pairs;

	std::mutex lock;

	SocketVector();

	void AddPair(SOCKET& _instructionSocket, SOCKET& _dataSocket);
	SocketPair& GetPair(SOCKET& _instructionSocket);
	void RemovePair(SOCKET& _instructionSocket);

};

