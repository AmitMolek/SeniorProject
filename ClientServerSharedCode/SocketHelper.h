#pragma once

#include <iostream>
#include <WinSock2.h>
#include <string>

class SocketHelper {
public:
	static void InitWinSock();

	static struct addrinfo* GetAddressInfo(PCSTR serverAddress, PCSTR connectionPort, struct addrinfo hints);
	static void GetSocket(struct addrinfo* result, SOCKET& outputSocket);

	template<typename F>
	static void SendAll(const SOCKET& socket, char* buffer, int length, F action) throw();
	static void SendAll(const SOCKET& socket, char* buffer, int length) throw();
	static void SendMsg(SOCKET& socket, std::string msg) throw();
};

