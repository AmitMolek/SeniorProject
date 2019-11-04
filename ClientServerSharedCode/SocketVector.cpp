#include "SocketVector.h"

#include <WinSock2.h>
#include <mutex>
#include <vector>

SocketVector::SocketVector() : pairs(), lock(){

}

void SocketVector::AddPair(SOCKET& _instructionSocket, SOCKET& _dataSocket) {
	lock.lock();
	pairs.push_back({_instructionSocket, _dataSocket});
	lock.unlock();
}

SocketPair& SocketVector::GetPair(SOCKET& _instructionSocket){
	for (SocketPair& pair : pairs) {
		if (pair.instructionSocket == _instructionSocket) {
			return pair;
		}
	}
}