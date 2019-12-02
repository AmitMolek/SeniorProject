#include "FirstFit.h"
#include <vector>

std::string FirstFit::GetName() {
	return "FirstFit";
}

double FirstFit::CalculateMean(size_t input_length) {
	return 1.0;
}

int FirstFit::RunAlgorithm(unsigned long long int fileSize, std::vector<unsigned long long int> freeCapacity) {
	for (int i = 0; i < freeCapacity.size(); i++) {
		if (fileSize <= freeCapacity[i])
			return i;
	}
	
	return -1;
}