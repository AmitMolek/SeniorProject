#include "SocketHelper.h"
#include "BaseServer.h"

#define INSTRUCTION_PORT "23456"
#define DATA_PORT "23457"

int main() {
	SocketHelper::InitWinSock();
	BaseServer bs{INSTRUCTION_PORT, DATA_PORT};

	while (true);
}