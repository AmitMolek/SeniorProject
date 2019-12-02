#pragma once
#include "VContainer.h"

#include <string>
#include <vector>

class IBPAlgorithm {
public:
	virtual std::string GetName() = 0;
	virtual double CalculateMean(size_t input_length) = 0;
	// Returns the index of the chosen place to store the file, -1 if not found
	virtual int RunAlgorithm(uint64_t fileSize, std::vector<uint64_t> freeCapacity) = 0;
	virtual int RunAlgorithm(uint64_t fileSize, std::vector<VContainer>& containers) = 0;
};

