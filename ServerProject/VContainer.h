#pragma once

#include "VFile.h"
#include "StorageObject.h"

#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

class VContainer : public StorageObject{
public:
	bool isContainerClosed;
	std::vector<VFile> files;

	VContainer(const VContainer& other) = delete;
	VContainer(VContainer&& other) noexcept;
	VContainer(fs::path _rootPath, unsigned long long int _capacity, StorageObject* _parent = nullptr);

	// Opens the container
	void OpenContainer();
	// Closes the container
	void CloseContainer();
	// Creates the folder if it doesnt exists
	bool CreateContainerFolder();
};

