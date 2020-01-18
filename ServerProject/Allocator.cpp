#include "Allocator.h"
#include "ConsoleOutput.h"

#define MAX_INPUT_WEIGHT 10
#define MAX_ITEMS_IN_BIN 10
#define MAX_BIN_SIZE 10
#define SERIES_LEGTH  2
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


 IBPAlgorithm* Allocator:: GetStorageAlgorithmBySeries(std::vector<VContainer> &containers, std::vector<IBPAlgorithm*> algorithms)
{
	 
	int ans = 0, anchor = 0;
	uint64_t iMinus1usedCapacity;
	uint64_t iusedCapacity;
	for (int i = 0; i < containers.size(); ++i)
	{
		if (i > 0)
		{
			iMinus1usedCapacity = containers[i - 1].capacity - containers[i - 1].GetFreeCapacity();
			iusedCapacity = containers[i].capacity - containers[i].GetFreeCapacity();
		}
		
		if (i > 0 && iMinus1usedCapacity >= iusedCapacity) anchor = i;
		ans = std::max(ans, i - anchor + 1);
	}
	if (ans >= SERIES_LEGTH)
	{
		for (int i = 0; i < algorithms.size(); i++)
		{
			if (algorithms[i]->GetName()._Equal("BestFit"))
			{
				return algorithms[i];
			}
		}

	}
	else
	{
		for (int i = 0; i < algorithms.size(); i++)
		{
			if (algorithms[i]->GetName()._Equal("FirstFit"))
			{
				return algorithms[i];
			}
		}
	}
		
	 
}