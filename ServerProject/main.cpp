#include "SocketHelper.h"
#include "BaseServer.h"
#include "VStorage.h"

#include "IBPAlgorithm.h"
#include "FirstFit.h"
#include "NextFit.h"
#include "BestFit.h"

#include "Array3D.h"
#include "dbHandler.h"

#define INSTRUCTION_PORT "23456"
#define DATA_PORT "23457"

int main() {
	SocketHelper::InitWinSock();
	
	fs::path virtualStorageRoot = fs::current_path();
	virtualStorageRoot /= "VirtualStorage";
	
	unsigned int numOfContainers = 1;
	FirstFit ff;
	NextFit nf;
	BestFit bf;
	VStorage virtualStorage(virtualStorageRoot, { &ff }, true);
	BaseServer bs{INSTRUCTION_PORT, DATA_PORT, virtualStorage};
	
	while (true);
	std::cout << "[INFO] Main thread finished\n";
}