#include "VStorage.h"
#include "VStorageHelper.h"

VStorage::VStorage(fs::path _rootPath, unsigned long long int _capacity, unsigned int _containersCount) : 
	StorageObject(_rootPath), containers() {
	containersCount = _containersCount;
}

void VStorage::CreateContainers(std::vector<VContainer>& _containers, unsigned int count) {
	std::vector<fs::path> paths = VStorageHelper::GetSerialPaths(GetPath(), count);
	for (fs::path p : paths){
		VContainer cont(p, 1000, 0, this);
		_containers.push_back(std::move(cont));
	}
}

void VStorage::AllocateFile(VFile& vFile, FileUploadInfo& _fileInfo){
	CreateContainers(containers, 3);
	fs::path filePath = containers[0].GetPath();
	filePath /= _fileInfo.fileName;

	vFile = VFile(filePath, _fileInfo.fileName, _fileInfo.fileSize);
}

void VStorage::CreatePathFolders() {
	if (!fs::exists(GetPath()))
		fs::create_directories(GetPath());
}