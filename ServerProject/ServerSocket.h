#pragma once
#include "BasicSocket.h"

class ServerSocket : public BasicSocket {
public:

	ServerSocket();
	ServerSocket(PCSTR _serverAddress, PCSTR _connectionPort);

	void BindSocket();
	void ListenToSocket();
	void AcceptSingleConnection();
	void AcceptConnections();
	void AcceptTwoSocketsConnections();

	void FillHints();
	void Disconnect();

};

