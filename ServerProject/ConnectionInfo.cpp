#include "ConnectionInfo.h"

ConnectionInfo::ConnectionInfo(SOCKET* _instructionSocket, 
							   SOCKET* _dataSocket, 
							   std::string _clientAddress,
							   VStorage* _storage) {
	instructionSocket = _instructionSocket;
	dataSocket = _dataSocket;
	clientAddress = _clientAddress;
	storage = _storage;
}
