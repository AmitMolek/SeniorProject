#include "SocketHelper.h"
#include "BaseServer.h"
#include "VStorage.h"

#include "IBPAlgorithm.h"
#include "FirstFit.h"
#include "NextFit.h"
#include "BestFit.h"

#include "dbHandler.h"

#include <fstream>
#include <sstream>

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
	
	VStorage virtualStorage(virtualStorageRoot, { &ff, &nf ,&bf }, true);
	virtualStorage.SetSystemParams(files_distribution, 
								   max_items_in_container, 
								   container_size);
	BaseServer bs{INSTRUCTION_PORT, DATA_PORT, virtualStorage};
	
	int i;
	std::cin >> i;
	std::cout << "[INFO] Main thread finished\n";

	//fs::path inputPath = fs::current_path();
	//inputPath /= "TestFiles";
	//inputPath /= "Input";
	//inputPath /= "test.jpg";

	//fs::path outputPath = fs::current_path();
	//outputPath /= "TestFiles";
	//outputPath /= "test.jpg";

	//std::cout << "Input path: " << inputPath << std::endl;
	//std::cout << "Output path: " << outputPath << std::endl;

	//std::ifstream input(inputPath, std::ios::binary);
	//std::ofstream output(outputPath, std::ios::binary);

	////std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});
	//output << input.rdbuf();

	//output.close();
	//output.flush();
	//input.close();

	//int i;
	//std::cin >> i;
}