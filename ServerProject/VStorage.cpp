#include "VStorage.h"
#include "VStorageHelper.h"
#include "Allocator.h"

#include <utility>

VStorage::VStorage(fs::path _rootPath, 
				   unsigned int _containersCount, 
				   std::vector<IBPAlgorithm*> _algorithms) :
	StorageObject(_rootPath), containers(), algorithms(std::move(_algorithms)) {
	containersCount = _containersCount;

	CreateContainers(containers, 3);
}

void VStorage::CreateContainers(std::vector<VContainer>& _containers, unsigned int count) {
	std::vector<fs::path> paths = VStorageHelper::GetSerialPaths(GetPath(), count);
	for (fs::path p : paths){
		VContainer cont(p, 2 * 1024 * 1024, 0, this);
		_containers.push_back(std::move(cont));
	}
}

void VStorage::AllocateFiles(std::vector<std::pair<VFile&, FileUploadInfo&>> files) {
	std::vector<IBPAlgorithm*> algosPtr;
	for (IBPAlgorithm* algo : algorithms)
		algosPtr.push_back(algo);
	std::cout << "got here 1\n";
	IBPAlgorithm* algo = Allocator::GetStorageAlgorithm(algosPtr, files.size());
	for(auto& pair : files){
		std::cout << "got here 2\n";
		std::vector<unsigned long long int> containersFreeCapacity;
		for (VContainer& cont : containers) {
			containersFreeCapacity.push_back(cont.GetFreeCapacity());
			std::cout << "Container free: " << cont.GetFreeCapacity() << "\n";
		}

		int contIndex = algo->RunAlgorithm(pair.second.fileSize, containersFreeCapacity);
		std::cout << "Contaienr index = " << contIndex << "\n";
		if (contIndex > -1) {
			VContainer& parentCont = containers[contIndex];
			fs::path filePath = parentCont.GetPath();
			filePath /= pair.second.fileName;
			pair.first.SetPath(filePath);
			pair.first.fileName = pair.second.fileName;
			pair.first.fileSize = pair.second.fileSize;
			parentCont.UseCapacity(pair.first.fileSize);
			std::cout << "stored file " << pair.first.GetPath() << ","
				<< pair.first.fileName << "," << pair.first.fileSize << "\n";
		}
	}
}

//void VStorage::AllocateFiles(std::vector<std::pair<VFile&, FileUploadInfo&>> files) {
//	CreateContainers(containers, 3);
//
//	std::vector<IBPAlgorithm*> algosPtr;
//	for (IBPAlgorithm& algo : algorithms)
//		algosPtr.push_back(&algo);
//
//	VContainer* cont = Allocator::AllocateContainer(files[0].second.fileSize, algosPtr, containers, files.size());
//	for (auto& pair : files) {
//		cont->UseCapacity(pair.second.fileSize);
//		fs::path filePath = cont->GetPath();
//		pair.first.SetPath(filePath);
//	}
//}

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