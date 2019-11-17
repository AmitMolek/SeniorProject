#pragma once

#include "VFolder.h"

#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

class VContainer {
public:
	fs::path rootPath;
	unsigned long long int capacity;
	bool isContainerClosed;
	std::vector<VFile> files;

	VContainer(const VContainer& other) = delete;
	VContainer(VContainer&& other);
	VContainer(fs::path _rootPath, unsigned long long int _capacity);

	void OpenContainer();
	void CloseContainer();
};

