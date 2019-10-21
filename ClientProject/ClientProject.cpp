#include <iostream>

#include "BaseSocket.h"
#include "ClientSocket.h"

void main() {
	ClientSocket::InitWinsock();

	ClientSocket clientSocket;
}