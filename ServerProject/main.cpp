#include "SocketHelper.h"
#include "BaseServer.h"
#include "VStorage.h"

#include "IBPAlgorithm.h"
#include "FirstFit.h"
#include "NextFit.h"

#define INSTRUCTION_PORT "23456"
#define DATA_PORT "23457"

int main() {
	SocketHelper::InitWinSock();
	
	fs::path virtualStorageRoot = fs::current_path();
	virtualStorageRoot /= "VirtualStorage";
	
	unsigned int numOfContainers = 1;
	FirstFit ff;
	VStorage virtualStorage(virtualStorageRoot, numOfContainers, {&ff});
	BaseServer bs{INSTRUCTION_PORT, DATA_PORT, virtualStorage};
	
	while (true);

	//VStorage virtualStorage(fs::current_path(), 100, 10);
	//std::vector<DBAlgorithm> algorithms = {FirstFit(), NextFit()};
	//for (auto pair : Allocator::CalculateMean(algorithms)) {
	//	std::cout << pair.second << "\n";
	//}
	//unsigned long long int fileSize = 100;
	//std::vector<VContainer> conts;
	//conts.push_back({ "a", 50 });
	//conts.push_back({ "b", 200 });

	//FirstFit ff;
	//std::vector<IBPAlgorithm*> algos {&ff};
	//VContainer* c = Allocator::AllocateContainer(fileSize, algos, conts, 1);
	//if (c)
	//	std::cout << c->GetPath() << "\n";
	//else std::cout << "Need to open new container\n";
}