#pragma once

#include "VFile.h"
#include "StorageObject.h"

#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

class VContainer : public StorageObject{
public:
	bool isContainerClosed;
	unsigned long long int capacity;
	unsigned long long int usedCapacity;
	std::vector<VFile> files;

	VContainer(const VContainer& other) = delete;
	VContainer(VContainer&& other) noexcept;
	VContainer(fs::path _rootPath, 
			   unsigned long long int _capacity, 
			   unsigned long long int _usedCapacity = 0, 
			   StorageObject* _parent = nullptr);

	// Opens the container
	void OpenContainer();
	// Closes the container
	void CloseContainer();
	// Creates the folder if it doesnt exists
	bool CreateContainerFolder();

	unsigned long long int GetTotalCapacity();
	unsigned long long int GetFreeCapacity();
	unsigned long long int GetUsedCapacity();

	void SetTotalCapacity(unsigned long long int _capacity);
	void SetUsedCapacity(unsigned long long int _usedCapacity);

	bool CanStore(unsigned long long int toStore);
	bool UseCapacity(unsigned long long toUse);
};

