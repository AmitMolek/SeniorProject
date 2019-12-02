#include "FirstFit.h"
#include <vector>

std::string FirstFit::GetName() {
	return "FirstFit";
}

double FirstFit::CalculateMean(size_t input_length) {
	return 1.0;
}

int FirstFit::RunAlgorithm(uint64_t fileSize, std::vector<VContainer>& containers) {
	for (size_t i = 0; i < containers.size(); i++) {
		if (containers[i].CanStore(fileSize))
			return i;
	}

	return -1;
}