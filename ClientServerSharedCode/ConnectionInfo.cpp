#include "ConnectionInfo.h"

ConnectionInfo::ConnectionInfo(SOCKET* _instructionSocket, SOCKET* _dataSocket, std::string _clientAddress){
	instructionSocket = _instructionSocket;
	dataSocket = _dataSocket;
	clientAddress = _clientAddress;
}
