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

#define INSTRUCTION_PORT "23456"
#define DATA_PORT "23457"

int main() {
	//SocketHelper::InitWinSock();
	//
	//fs::path virtualStorageRoot = fs::current_path();
	//virtualStorageRoot /= "VirtualStorage";
	//
	//unsigned long long int capacity = 100 * 1024;
	//unsigned int numOfContainers = 4;
	//
	//VStorage virtualStorage(virtualStorageRoot, capacity, numOfContainers);
	//BaseServer bs{INSTRUCTION_PORT, DATA_PORT, virtualStorage};
	//
	//while (true);

	//FileUploadInfo fileInfo("gay.txt", 500);
	//VFile file;
	//virtualStorage.AllocateFile(file, fileInfo);
	//std::cout << file << "\n";
	//VFile file2 = std::move(file);
	//std::cout << file2 << "\n";
	//std::cout << file << "\n";
	StorageObject p(fs::current_path(), 10, 0);
	VContainer a(fs::current_path(), 100, &p);
	std::cout << a << "\n";
	VContainer b(std::move(a));
	std::cout << b << "\n";
	std::cout << a << "\n";
}