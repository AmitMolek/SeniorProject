#pragma once
#include "IBPAlgorithm.h"

class NextFit : public IBPAlgorithm {
public:
	std::string GetName();
	double CalculateMean(size_t input_length);
	// Returns the index of the chosen place to store the file, -1 if not found
	int RunAlgorithm(uint64_t fileSize, std::vector<VContainer>& containers);
};

