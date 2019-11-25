#include "VContainer.h"

VContainer::VContainer(VContainer&& other) noexcept :
	StorageObject(std::move(other)), 
	isContainerClosed(std::exchange(other.isContainerClosed, false)), 
	files(std::move(other.files)), 
	capacity(std::exchange(other.capacity, 0)),
	usedCapacity(std::exchange(other.usedCapacity, 0)) {
}

VContainer::VContainer(fs::path _rootPath,
		   unsigned long long int _capacity,
		   unsigned long long int _usedCapacity,
		   StorageObject* _parent) : StorageObject(_rootPath, _parent), files(){
	capacity = _capacity;
	usedCapacity = _usedCapacity;
	isContainerClosed = false;
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

unsigned long long int VContainer::GetTotalCapacity() {
	return capacity;
}

unsigned long long int VContainer::GetFreeCapacity() {
	return (GetTotalCapacity() - GetUsedCapacity());
}

unsigned long long int VContainer::GetUsedCapacity() {
	return usedCapacity;
}

void VContainer::SetTotalCapacity(unsigned long long int _capacity) {
	capacity = _capacity;
}

void VContainer::SetUsedCapacity(unsigned long long int _usedCapacity) {
	usedCapacity = _usedCapacity;
}

bool VContainer::CanStore(unsigned long long int toStore) {
	return (GetTotalCapacity() > (GetUsedCapacity() + toStore)) ? true : false;
}

bool VContainer::UseCapacity(unsigned long long toUse) {
	if (CanStore(toUse)) {
		usedCapacity += toUse;
		return true;
	} else return false;
}