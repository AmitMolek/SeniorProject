#pragma once
#include "IBPAlgorithm.h"

class NextFit : public IBPAlgorithm {
public:
	std::string GetName();
	double CalculateMean(size_t input_length);
};

