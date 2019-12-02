#include "VStorage.h"
#include "VStorageHelper.h"
#include "Allocator.h"
#include "dbHandler.h"
#include "ConsoleOutput.h"

#include <utility>

constexpr uint64_t CONTAINER_SIZE = 10 * 1024 * 1024;

namespace db = dbHandler;

VStorage::VStorage(fs::path _rootPath, 
				   unsigned int _containersCount, 
				   std::vector<IBPAlgorithm*> _algorithms) :
	StorageObject(_rootPath), containers(), algorithms(std::move(_algorithms)) {
	containersCount = 0;

	CreatePathFolders();
	CreateContainers(containers, _containersCount);
}

void VStorage::CreateContainer(unsigned int id, uint64_t _capacity, StorageObject* parent){
	VContainer cont;
	fs::path contPath = parent->GetPath();
	contPath /= std::to_string(id);
	cont.SetPath(contPath);
	cont.SetTotalCapacity(_capacity);
	cont.SetUsedCapacity(0);
	cont.SetParent(parent);

	if (!fs::exists(cont.GetPath()))
		fs::create_directory(cont.GetPath());
	ConsoleOutput() << "[INFO] Created container " << cont.GetPrint() << "\n";
	db::Database::Instance() << &cont;
	containers.push_back(std::move(cont));
	containersCount++;
}

void VStorage::CreateContainers(std::vector<VContainer>& _containers, unsigned int count) {
	db::Database::Instance().retrieveContainers(_containers);
	/*for (unsigned int i = 0; i < count; i++) {
		CreateContainer(i, (unsigned)(CONTAINER_SIZE), this);
	}*/
}

void VStorage::AllocateFiles(std::vector<std::pair<VFile&, FileUploadInfo&>> files) {
	std::vector<IBPAlgorithm*> algosPtr;
	for (IBPAlgorithm* algo : algorithms)
		algosPtr.push_back(algo);

	IBPAlgorithm* algo = Allocator::GetStorageAlgorithm(algosPtr, files.size());
	for(auto& pair : files){
		std::vector<uint64_t> containersFreeCapacity;
		for (VContainer& cont : containers) {
			if (cont.GetTotalCapacity() >= pair.second.fileSize)
				containersFreeCapacity.push_back(cont.GetFreeCapacity());
		}
		if (containersFreeCapacity.size() == 0){
			ConsoleOutput() << "[INFO] File size cannot be stored " 
				<< "[" << pair.second.fileName << "," << pair.second.fileSize << "]\n";
		}

		//int contIndex = algo->RunAlgorithm(pair.second.fileSize, containersFreeCapacity);
		int contIndex = algo->RunAlgorithm(pair.second.fileSize, containers);

		VContainer* parentCont;
		if (contIndex > -1) {
			parentCont = &containers[contIndex];
		} else {
			CreateContainer(containersCount, (unsigned)(CONTAINER_SIZE), this);
			parentCont = &containers[containersCount - 1];
		}

		fs::path filePath = parentCont->GetPath();
		filePath /= pair.second.fileName;
		pair.first.SetPath(filePath);
		pair.first.fileName = pair.second.fileName;
		pair.first.fileSize = pair.second.fileSize;
		parentCont->UseCapacity(pair.first.fileSize);
		ConsoleOutput() << "[INFO] Stored " << pair.first << " in container " << parentCont->GetPrint() << "\n";
	}
}

//void VStorage::AllocateFile(VFile& vFile, FileUploadInfo& _fileInfo){
//	CreateContainers(containers, 3);
//
//	fs::path filePath = containers[0].GetPath();
//	filePath /= _fileInfo.fileName;
//
//	vFile = VFile(filePath, _fileInfo.fileName, _fileInfo.fileSize);
//}

void VStorage::CreatePathFolders() {
	if (!fs::exists(GetPath()))
		fs::create_directories(GetPath());
}