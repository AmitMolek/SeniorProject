#include "VStorage.h"
#include "VStorageHelper.h"

VStorage::VStorage(fs::path _rootPath, unsigned int _capacity, unsigned int containersCount) : containers(){
	rootPath = _rootPath;
	capacity = _capacity;
}

void VStorage::CreateContainers(std::vector<VContainer>& _containers, unsigned int count) {
	std::vector<fs::path> paths = VStorageHelper::GetSerialPaths(rootPath, count);
	for (fs::path p : paths){
		VContainer cont(p, 1000);
		_containers.push_back(std::move(cont));
	}
}

void VStorage::AllocateFile(VFile& vFile, FileUploadInfo& _fileInfo){
	CreateContainers(containers, 3);
	fs::path filePath = containers[0].rootPath;
	filePath /= _fileInfo.fileName;

	vFile = VFile(filePath, _fileInfo.fileName, _fileInfo.fileSize);
}