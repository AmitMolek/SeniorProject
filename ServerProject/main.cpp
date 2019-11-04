#include "SocketHelper.h"
#include "BaseServer.h"

int main() {
	SocketHelper::InitWinSock();
	BaseServer bs{"23456", "23457"};
}