#pragma once

#include <string>
#include <vector>

class IBPAlgorithm {
public:
	virtual std::string GetName() = 0;
	virtual double CalculateMean(size_t input_length) = 0;
	// Returns the index of the chosen place to store the file, -1 if not found
	virtual int RunAlgorithm(unsigned long long int fileSize, std::vector<unsigned long long int> freeCapacity) = 0;
};

