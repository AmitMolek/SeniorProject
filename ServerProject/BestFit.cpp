#include "BestFit.h"

#include <limits>

std::string BestFit::GetName() {
	return "BestFit";
}

double BestFit::CalculateMean(size_t input_length) {
	return 3.0;
}

int BestFit::RunAlgorithm(uint64_t fileSize, std::vector<uint64_t> freeCapacity) {
	return -1;
}

int BestFit::RunAlgorithm(uint64_t fileSize, std::vector<VContainer>& containers) {
	int containerIndex = -1;
	uint64_t minSize = std::numeric_limits<uint64_t>::max();
	for (size_t i = 0; i < containers.size(); i++) {
		if (containers[i].CanStore(fileSize)) {
			uint64_t minCapacityLeft = containers[i].GetTotalCapacity() - fileSize;
			if (minSize < minCapacityLeft) {
				containerIndex = i;
				minSize = minCapacityLeft;
			}
		}
	}

	return containerIndex;
}