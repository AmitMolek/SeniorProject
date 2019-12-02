#pragma once

#include "VContainer.h"
#include "FileUploadInfo.h"
#include "StorageObject.h"
#include "Allocator.h"
#include "IBPAlgorithm.h"

#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

//TODO
// ALLOCATE FILE USING THE NEW ALGORITHM SYSTEM

class VStorage : public StorageObject{
public:

	unsigned int containersCount;
	std::vector<VContainer> containers;
	std::vector<IBPAlgorithm*> algorithms;

	VStorage(fs::path _rootPath, 
			 unsigned int _containersCount, 
			 std::vector<IBPAlgorithm*> _algorithms);

	void AllocateFile(VFile& vFile, FileUploadInfo& _fileInfo);
	void AllocateFiles(std::vector<std::pair<VFile&, FileUploadInfo&>> files);
private:
	void CreateContainers(std::vector<VContainer>& _containers, unsigned int count);

	void CreatePathFolders();
};

