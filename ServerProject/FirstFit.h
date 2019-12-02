#pragma once
#include "IBPAlgorithm.h"

class FirstFit : public IBPAlgorithm {
public:
	std::string GetName();
	double CalculateMean(size_t input_length);
	int RunAlgorithm(uint64_t fileSize, std::vector<VContainer>& containers);
};

