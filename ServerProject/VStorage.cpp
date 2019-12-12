#include "VStorage.h"
#include "VStorageHelper.h"
#include "Allocator.h"
#include "dbHandler.h"
#include "ConsoleOutput.h"

#include <utility>

constexpr uint64_t CONTAINER_SIZE = 10 * 1024 * 1024;

namespace db = dbHandler;

VStorage::VStorage(fs::path _rootPath,
				   std::vector<IBPAlgorithm*> _algorithms,
				   bool loadFromDB,
				   unsigned int _containersCount) :
	StorageObject(_rootPath), containers(), algorithms(std::move(_algorithms)) {
	//CreatePathFolders();
	containersCount = 0;
	CreatePath();

	if (loadFromDB) {
		db::Database::Instance().retrieveContainers(containers);
		containersCount = containers.size();
	} else {
		CreateContainers(containers, _containersCount);
	}

	input_files_weight_distribution = 0;
	max_items_in_container = 0;
	max_container_size = 0;
}

void VStorage::SetSystemParams(size_t weight_dist,
					 size_t maxItemsInContainer,
					 size_t maxContainerSize) {
	input_files_weight_distribution = weight_dist;
	max_items_in_container = maxItemsInContainer;
	max_container_size = maxContainerSize;
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
	for (unsigned int i = 0; i < count; i++)
		CreateContainer(i, (uint64_t)(max_container_size), this);
}

void VStorage::AllocateFiles(std::vector<std::pair<VFile&, FileUploadInfo&>> files) {
	std::vector<IBPAlgorithm*> algosPtr;
	for (IBPAlgorithm* algo : algorithms)
		algosPtr.push_back(algo);

	size_t totalFilesUploaded = 0;
	for (VContainer& cont : containers) {
		totalFilesUploaded += cont.files.size();
	}
	totalFilesUploaded += files.size();

	//IBPAlgorithm* algo = Allocator::GetStorageAlgorithm(algosPtr, files.size());
	ConsoleOutput() << "[INFO] Total files uploaded so far " << totalFilesUploaded << "\n";
	IBPAlgorithm* algo = Allocator::GetStorageAlgorithm(algosPtr, 
														totalFilesUploaded, 
														input_files_weight_distribution,
														max_items_in_container,
														max_container_size);
	db::Database::Instance().setCurrentAlgorithm(algo->GetName());
	
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

		int contIndex = algo->RunAlgorithm(pair.second.fileSize, containers);

		VContainer* parentCont;
		if (contIndex > -1) {
			parentCont = &containers[contIndex];
		} else {
			CreateContainer(containersCount, (uint64_t)(CONTAINER_SIZE), this);
			parentCont = &containers[containersCount - 1];
		}

		fs::path filePath = parentCont->GetPath();
		filePath /= pair.second.fileName;
		pair.first.SetPath(filePath);
		pair.first.fileName = pair.second.fileName;
		pair.first.fileSize = pair.second.fileSize;
		pair.first.SetParent(parentCont);
		//parentCont->files.push_back(std::move(pair.first));
		//parentCont->UseCapacity(pair.first.fileSize);
		ConsoleOutput() << "[INFO] Stored " << pair.first << " in container " << parentCont->GetPrint() << "\n";
	}
}

void VStorage::CreatePathFolders() {
	if (!fs::exists(GetPath()))
		fs::create_directories(GetPath());
}