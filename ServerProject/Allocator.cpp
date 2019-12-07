#include "Allocator.h"
#include "ConsoleOutput.h"

#define MAX_INPUT_WEIGHT 10
#define MAX_ITEMS_IN_BIN 10
#define MAX_BIN_SIZE 10

//VContainer* Allocator::AllocateContainer(unsigned long long int fileSize, std::vector<IBPAlgorithm*> algorithms, std::vector<VContainer>& containers, size_t input_length) {
//	IBPAlgorithm* storageAlgo = Allocator::GetStorageAlgorithm(algorithms, input_length);
//	std::vector<unsigned long long int> freeCapacity;
//	for (auto& cont : containers)
//		freeCapacity.push_back(cont.GetFreeCapacity());
//	int containerIndex = storageAlgo->RunAlgorithm(fileSize, containers);
//
//	if (containerIndex > 0)
//		return &containers[containerIndex];
//
//	return nullptr;
//}

bool CompareAlgorithms(std::pair<IBPAlgorithm*, double> a, std::pair<IBPAlgorithm*, double> b){
	return (a.second < b.second);
}

IBPAlgorithm* Allocator::GetStorageAlgorithm(
	std::vector<IBPAlgorithm*> algorithms,
	size_t input_length,
	size_t max_input_weight,
	size_t max_items_in_bin,
	size_t max_bin_size) {
	auto values = Allocator::CalculateMeans(algorithms, 
											input_length, 
											max_input_weight, 
											max_items_in_bin, 
											max_bin_size);
	std::sort(values.begin(), values.end(), CompareAlgorithms);
	
	ConsoleOutput() << "[Algorithm] Used algorithm " << values[0].first->GetName()
		<< " with Mean value = " << values[0].second << "\n";
	return values[0].first;
}

std::vector<std::pair<IBPAlgorithm*, double>> Allocator::CalculateMeans(
	std::vector<IBPAlgorithm*> algorithms, 
	size_t input_length, 
	size_t max_input_weight, 
	size_t max_items_in_bin, 
	size_t max_bin_size) {
	std::vector<std::pair<IBPAlgorithm*, double>> meanValues;
	for (IBPAlgorithm* algo : algorithms)
		meanValues.push_back({algo, algo->CalculateMean(input_length, 
														max_input_weight,
														max_items_in_bin,
														max_bin_size)});

	return meanValues;
}
