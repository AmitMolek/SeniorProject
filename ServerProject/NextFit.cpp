#include "NextFit.h"

std::string NextFit::GetName() {
	return "NextFit";
}

double NextFit::CalculateMean(size_t input_length) {
	return 2.0;
}

int NextFit::RunAlgorithm(uint64_t fileSize, std::vector<VContainer>& containers) {
	for (size_t i = 0; i < containers.size(); i++) {
		// if container is closed continue to the next one
		if (containers[i].isContainerClosed)
			continue;
		// if the container can store the file
		if (containers[i].CanStore(fileSize))
			return i;

		// if the container cant store the file, close it
		containers[i].CloseContainer();
	}

	return -1;
}

int NextFit::RunAlgorithm(uint64_t fileSize, std::vector<uint64_t> freeCapacity) {
	return -1;
}