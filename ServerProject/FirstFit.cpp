#include "FirstFit.h"

#include <vector>

std::string FirstFit::GetName() {
	return "FirstFit";
}

double FirstFit::calculate_x(Array3D<double>& x_arr,
				   size_t input_length, 
				   size_t max_bin_size, 
				   size_t max_items_in_bin,
				   size_t max_input_weight){
	// Calculates and insert the start conditions to the array
	// X(1, m, 1) = (m/M), m=[0,M]
	// X(n, m, 0) = (1 - (m/M))^(n), n=[0, N], m=[0,M]
	for (size_t n = 0; n <= input_length; n++) {
		for (size_t m = 0; m <= max_bin_size; m++) {
			x_arr(n, m, 0) = pow(1.0 - (((double)m) / ((double)max_input_weight)), n);
			if (n == 1)
				x_arr(1, m, 1) = (((double)m) / ((double)max_input_weight));
		}
	}

	// Filling the array with all the dynamic programming calculations
	// X(n, m, i) = 
	//				SIGMA(j=1, m){ (1/M) * X(n-1, m-j, i-1) } + 
	//				SIGMA(j=m+1, M){ (1/M) * X(n-1, m, i) }
	for (size_t n = 1; n <= input_length; n++) {
		for (size_t m = 1; m <= max_bin_size; m++) {
			for (size_t i = 1; i <= max_items_in_bin; i++) {
				// We did this calculations in the start conditions
				// (i > n) ther cannot be i items in the bin if there only left n items in the input
				if (i == 1 && n == 1 || i > n) continue;

				// The first SIGMA
				for (size_t j = 1; j <= m; j++)
					if (m - j >= 0)
						x_arr(n, m, i) += (x_arr(n - 1, m - j, i - 1) / ((double)max_input_weight));

				// The second SIGMA
				for (size_t j = m + 1; j <= max_input_weight; j++)
					x_arr(n, m, i) += (x_arr(n - 1, m, i) / ((double)max_input_weight));
			}
		}
	}

	return x_arr(input_length, max_bin_size, max_items_in_bin);
}

double FirstFit::calculate_y(Array3D<double>& y_arr,
				   Array3D<double>& x_arr, 
				   size_t input_length, 
				   size_t open_bins, 
				   size_t max_bin_size,
				   size_t max_input_weight){
	// Y(n, k) = SIGMA(j=1, M){ X(n, M,j) * Y(n-j, k-1) }
	// With Starting condiotions:
	//	- Y(0,0) = Y(1,1) = 1
	//	- Y(1,k) = 0, k != 1
	//	- Y(n,0) = 0, n > 0

	// Setting the start conditions
	y_arr(0, 0, 0) = 1;
	y_arr(0, 1, 1) = 1;

	// Y(1,k) for every k != 1
	for (size_t k = 2; k <= open_bins; k++)
		y_arr(0, 1, k) = 0;

	// Y(n,0) for every n > 0
	for (size_t n = 1; n <= input_length; n++)
		y_arr(0, n, 0) = 0;

	// Calculating the sigma itself
	for (size_t n = 2; n <= input_length; n++)
		for (size_t k = 0; k <= open_bins; k++)
			for (size_t j = 1; j <= max_input_weight; j++) {
				if (n < j || k == 0) continue;
				y_arr(0, n, k) += x_arr(n, max_bin_size, j) * y_arr(0, n - j, k - 1);
			}

	return y_arr(0, input_length, open_bins);
}

double FirstFit::CalculateMean(size_t input_length,
							   size_t max_input_weight,
							   size_t max_items_in_bin,
							   size_t max_bin_size) {

	Array3D<double> x_arr(input_length + 1, max_input_weight + 1, max_input_weight + 1, 0);
	Array3D<double> y_arr(1, input_length + 1, input_length + 1, 0);
	Array3D<double> mean(1, 1, input_length + 1, 0);

	calculate_x(x_arr, input_length, max_bin_size, max_items_in_bin, max_input_weight);
	calculate_y(y_arr, x_arr, input_length, input_length, max_bin_size, max_input_weight);

	// Calculating the expected value (mean)
	// E(n) = SIGMA(j=1, n){ Y(n, j) * j }
	for (size_t j = 1; j <= input_length; j++)
		mean(0, 0, j) = y_arr(0, input_length, j) * j + mean(0, 0, j - 1);

	return mean(0, 0, input_length);
}

int FirstFit::RunAlgorithm(uint64_t fileSize, std::vector<VContainer>& containers) {
	for (size_t i = 0; i < containers.size(); i++) {
		if (containers[i].CanStore(fileSize))
			return i;
	}

	return -1;
}