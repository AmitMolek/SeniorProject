#pragma once

#include "VContainer.h"
#include "FileUploadInfo.h"
#include "StorageObject.h"
#include "Allocator.h"
#include "IBPAlgorithm.h"

#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

class VStorage : public StorageObject{
public:

	unsigned int containersCount;
	std::vector<VContainer> containers;
	std::vector<IBPAlgorithm*> algorithms;
	size_t input_files_weight_distribution;
	size_t max_items_in_container;
	size_t max_container_size;

	VStorage(fs::path _rootPath, 
			 std::vector<IBPAlgorithm*> _algorithms,
			 bool loadFromDB = false,
			 unsigned int _containersCount = 0);

	void AllocateFile(VFile& vFile, FileUploadInfo& _fileInfo);
	void AllocateFiles(std::vector<std::pair<VFile&, FileUploadInfo&>> files);
	void SetSystemParams(size_t weight_dist, 
						 size_t maxItemsInContainer, 
						 size_t maxContainerSize);
private:
	void CreateContainer(unsigned int id, uint64_t _capacity, StorageObject* parent);
	void CreateContainers(std::vector<VContainer>& _containers, unsigned int count);
	void CreatePathFolders();
};

