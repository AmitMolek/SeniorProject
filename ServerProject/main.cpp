#include "SocketHelper.h"
#include "BaseServer.h"
#include "VStorage.h"

#include "IBPAlgorithm.h"
#include "FirstFit.h"
#include "NextFit.h"
#include "BestFit.h"

#include "dbHandler.h"

#define INSTRUCTION_PORT "23456"
#define DATA_PORT "23457"

int main() {
	SocketHelper::InitWinSock();
	
	fs::path virtualStorageRoot = fs::current_path();
	virtualStorageRoot /= "VirtualStorage";
	
	FirstFit ff;
	NextFit nf;
	BestFit bf;

	size_t files_distribution = 10;
	size_t container_size = 10;
	size_t max_items_in_container = 10;

	VStorage virtualStorage(virtualStorageRoot, { &ff, &nf }, true);
	virtualStorage.SetSystemParams(files_distribution, 
								   max_items_in_container, 
								   container_size);
	BaseServer bs{INSTRUCTION_PORT, DATA_PORT, virtualStorage};

	int i;
	std::cin >> i;
	std::cout << "[INFO] Main thread finished\n";
}