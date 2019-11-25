#include "SocketHelper.h"
#include "BaseServer.h"

#include "VStorage.h"
#include <filesystem>

#include <sstream>
#include <fstream>
#include <istream>
#include <ostream>
#include "VFile.h"
#include "FileUploadInfo.h"
#include "StorageObject.h"

#include "dbHandler.h"

#define INSTRUCTION_PORT "23456"
#define DATA_PORT "23457"

int main() {
	SocketHelper::InitWinSock();
	
	fs::path virtualStorageRoot = fs::current_path();
	virtualStorageRoot /= "VirtualStorage";
	
	unsigned long long int capacity = 100 * 1024;
	unsigned int numOfContainers = 4;
	
	VStorage virtualStorage(virtualStorageRoot, capacity, numOfContainers);
	BaseServer bs{INSTRUCTION_PORT, DATA_PORT, virtualStorage};
	
	while (true);
}