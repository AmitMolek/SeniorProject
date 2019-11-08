#include "SocketVector.h"

#include <WinSock2.h>
#include <mutex>
#include <vector>

SocketVector::SocketVector() : sockets(), lock(){}

bool SocketVector::AddSocket(SOCKET _socket){
	return true;
}