#pragma once

#include "VFile.h"
#include "StorageObject.h"

#include <vector>
#include <filesystem>

namespace fs = std::experimental::filesystem;

class VContainer : public StorageObject{
public:
	bool isContainerClosed;
	uint64_t capacity;
	uint64_t usedCapacity;
	std::vector<VFile> files;

	VContainer();
	VContainer(const VContainer& other) = delete;
	VContainer(VContainer&& other) noexcept;
	VContainer(fs::path _rootPath, 
			   uint64_t _capacity, 
			   uint64_t _usedCapacity = 0,
			   StorageObject* _parent = nullptr);

	friend std::ostream& operator << (std::ostream& out, VContainer& obj);

	// Opens the container
	void OpenContainer();
	// Closes the container
	void CloseContainer();
	// Creates the folder if it doesnt exists
	bool CreateContainerFolder();

	uint64_t GetTotalCapacity();
	uint64_t GetFreeCapacity();
	uint64_t GetUsedCapacity();

	void SetTotalCapacity(uint64_t _capacity);
	void SetUsedCapacity(uint64_t _usedCapacity);

	bool CanStore(uint64_t toStore);
	bool UseCapacity(uint64_t toUse);

	std::string GetPrint();
};

