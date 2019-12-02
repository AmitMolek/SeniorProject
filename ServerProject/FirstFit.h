#pragma once
#include "IBPAlgorithm.h"

class FirstFit : public IBPAlgorithm {
public:
	std::string GetName();
	double CalculateMean(size_t input_length);
	int RunAlgorithm(unsigned long long int fileSize, std::vector<unsigned long long int> freeCapacity);
};

