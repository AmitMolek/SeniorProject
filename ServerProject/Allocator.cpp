#include "Allocator.h"

//TODO:
// UPDATE CONTAINER CAPACITY

VContainer* Allocator::AllocateContainer(unsigned long long int fileSize, std::vector<IBPAlgorithm*> algorithms, std::vector<VContainer>& containers, size_t input_length) {
	IBPAlgorithm* storageAlgo = Allocator::GetStorageAlgorithm(algorithms, input_length);
	std::vector<unsigned long long int> freeCapacity;
	for (auto& cont : containers)
		freeCapacity.push_back(cont.GetFreeCapacity());
	int containerIndex = storageAlgo->RunAlgorithm(fileSize, containers);

	if (containerIndex > 0)
		return &containers[containerIndex];

	return nullptr;
}

bool CompareAlgorithms(std::pair<IBPAlgorithm*, double> a, std::pair<IBPAlgorithm*, double> b){
	return (a.second < b.second);
}

IBPAlgorithm* Allocator::GetStorageAlgorithm(std::vector<IBPAlgorithm*> algorithms, size_t input_length) {
	auto values = Allocator::CalculateMeans(algorithms, input_length);
	std::sort(values.begin(), values.end(), CompareAlgorithms);

	return values[0].first;
}

std::vector<std::pair<IBPAlgorithm*, double>> Allocator::CalculateMeans(std::vector<IBPAlgorithm*> algorithms, size_t input_length) {
	std::vector<std::pair<IBPAlgorithm*, double>> meanValues;
	for (IBPAlgorithm* algo : algorithms)
		meanValues.push_back({algo, algo->CalculateMean(input_length)});

	return meanValues;
}
