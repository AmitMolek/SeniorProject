#pragma once

#include "VContainer.h"
#include "FileUploadInfo.h"

#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

class VStorage {
public:
	fs::path rootPath;
	unsigned int capacity;
	std::vector<VContainer> containers;

	VStorage(fs::path _rootPath, unsigned int _capacity, unsigned int containersCount);

	void AllocateFile(VFile& vFile, FileUploadInfo& _fileInfo);

private:
	void CreateContainers(std::vector<VContainer>& _containers, unsigned int count);
};

