#pragma once
#include "IBPAlgorithm.h"

class BestFit : public IBPAlgorithm {
	std::string GetName();
	double CalculateMean(size_t input_length);
	int RunAlgorithm(uint64_t fileSize, std::vector<uint64_t> freeCapacity);
	int RunAlgorithm(uint64_t fileSize, std::vector<VContainer>& containers);
};

