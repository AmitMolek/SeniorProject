#pragma once
#include "IBPAlgorithm.h"

class BestFit : public IBPAlgorithm {
	std::string GetName();
	double CalculateMean(size_t input_length,
						 size_t max_input_weight,
						 size_t max_items_in_bin,
						 size_t max_bin_size);
	int RunAlgorithm(uint64_t fileSize, std::vector<VContainer>& containers);
};

