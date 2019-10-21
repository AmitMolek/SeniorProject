#pragma once
#include "BasicSocket.h"

class ClientSocket : public BasicSocket {
public:

	ClientSocket();
	ClientSocket(PCSTR _serverAddress);

	void FillHints();
	void ConnectSocket();
	void Disconnect();
	void Send();
};

