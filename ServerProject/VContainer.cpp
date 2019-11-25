#include "VContainer.h"

VContainer::VContainer(VContainer&& other) noexcept :
	StorageObject(std::move(other)), 
	isContainerClosed(std::exchange(other.isContainerClosed, false)), 
	files(std::move(other.files)) {
}

VContainer::VContainer(fs::path _rootPath, unsigned long long int _capacity, StorageObject* _parent) : 
	StorageObject(_rootPath, _capacity, 0, _parent), files() {
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
	if (!fs::exists(GetPath())) {
		fs::create_directories(GetPath());
		return false;
	} else return true;
}
