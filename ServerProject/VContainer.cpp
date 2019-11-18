#include "VContainer.h"

//VContainer::VContainer(const VContainer& other) : 
//	capacity(other.capacity), 
//	isContainerClosed(other.isContainerClosed) {
//	rootPath = other.rootPath;
//	//capacity = other.capacity;
//	//isContainerClosed = other.isContainerClosed;
//	files = other.files;
//}

VContainer::VContainer(VContainer&& other) : 
	rootPath(std::move(other.rootPath)), 
	capacity(std::exchange(other.capacity, 0)), 
	isContainerClosed(std::exchange(other.isContainerClosed, false)), 
	files(std::move(other.files)) {}

VContainer::VContainer(fs::path _rootPath, unsigned long long int _capacity) : files() {
	rootPath = _rootPath;
	capacity = _capacity;

	isContainerClosed = false;

	CreateContainerFolder();
}

void VContainer::OpenContainer(){
	isContainerClosed = false;
}

void VContainer::CloseContainer() {
	isContainerClosed = true;
}

bool VContainer::CreateContainerFolder() {
	if (!fs::exists(rootPath)) {
		fs::create_directories(rootPath);
		return false;
	} else return true;
}
