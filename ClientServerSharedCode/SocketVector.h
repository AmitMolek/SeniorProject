#pragma once

#include <WinSock2.h>
#include <mutex>
#include <vector>

class SocketVector {
public:
	
	std::vector<SOCKET> sockets;
	std::mutex lock;

	SocketVector();

	bool AddSocket(SOCKET _socket);
	SOCKET GetSocket(int fd);
	bool RemoveSocket(int fd);

};

