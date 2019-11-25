#pragma once

#include "VContainer.h"
#include "FileUploadInfo.h"
#include "StorageObject.h"

#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

class VStorage : public StorageObject{
public:

	unsigned int containersCount;
	std::vector<VContainer> containers;

	VStorage(fs::path _rootPath, unsigned long long int _capacity, unsigned int _containersCount);

	void AllocateFile(VFile& vFile, FileUploadInfo& _fileInfo);

private:
	void CreateContainers(std::vector<VContainer>& _containers, unsigned int count);

	void CreatePathFolders();
};

