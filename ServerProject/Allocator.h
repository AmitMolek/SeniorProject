#pragma once
#include "VContainer.h"
#include "IBPAlgorithm.h"

#include <utility>

class Allocator {
public:
	static VContainer* AllocateContainer(unsigned long long int fileSize, std::vector<IBPAlgorithm*> algorithms, std::vector<VContainer>& containers, size_t input_length);
	static IBPAlgorithm* GetStorageAlgorithm(std::vector<IBPAlgorithm*> algorithms, size_t input_length);
	static std::vector<std::pair<IBPAlgorithm*, double>> CalculateMeans(std::vector<IBPAlgorithm*> algorithms, size_t input_length);
};

