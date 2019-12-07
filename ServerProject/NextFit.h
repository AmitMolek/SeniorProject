#pragma once
#include "IBPAlgorithm.h"
#include "Array3D.h"

class NextFit : public IBPAlgorithm {
public:
	std::string GetName();
	double CalculateMean(size_t input_length,
						 size_t max_input_weight,
						 size_t max_items_in_bin,
						 size_t max_bin_size);
	// Returns the index of the chosen place to store the file, -1 if not found
	int RunAlgorithm(uint64_t fileSize, std::vector<VContainer>& containers);
private:
	double calculate_x(Array3D<double>& x_arr,
					   size_t input_length,
					   size_t max_bin_size,
					   size_t max_items_in_bin,
					   size_t max_input_weight);
	double calculate_y(Array3D<double>& y_arr,
					   Array3D<double>& x_arr,
					   size_t input_length,
					   size_t open_bins,
					   size_t max_bin_size,
					   size_t max_input_weight);
};

