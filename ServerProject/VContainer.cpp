#include "VContainer.h"

VContainer::VContainer() : StorageObject("", nullptr), capacity(0), usedCapacity(0), files(), isContainerClosed(false){}

VContainer::VContainer(VContainer&& other) noexcept :
	StorageObject(std::move(other)), 
	isContainerClosed(std::exchange(other.isContainerClosed, false)), 
	files(std::move(other.files)), 
	capacity(std::exchange(other.capacity, 0)),
	usedCapacity(std::exchange(other.usedCapacity, 0)) {
}

VContainer::VContainer(fs::path _rootPath,
		   uint64_t _capacity,
		   uint64_t _usedCapacity,
		   StorageObject* _parent) : StorageObject(_rootPath, _parent), files(){
	capacity = _capacity;
	usedCapacity = _usedCapacity;
	isContainerClosed = false;

	CreatePath();
}

std::ostream& operator << (std::ostream& out, VContainer& obj) {
	out << "["
		<< obj.GetPath() << ","
		<< obj.GetTotalCapacity() << ","
		<< obj.GetUsedCapacity() <<
		"]";

	return out;
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

uint64_t VContainer::GetTotalCapacity() {
	return capacity;
}

uint64_t VContainer::GetFreeCapacity() {
	return (GetTotalCapacity() - GetUsedCapacity());
}

uint64_t VContainer::GetUsedCapacity() {
	return usedCapacity;
}

void VContainer::SetTotalCapacity(uint64_t _capacity) {
	capacity = _capacity;
}

void VContainer::SetUsedCapacity(uint64_t _usedCapacity) {
	usedCapacity = _usedCapacity;
}

bool VContainer::CanStore(uint64_t toStore) {
	return (GetTotalCapacity() > (GetUsedCapacity() + toStore)) ? true : false;
}

bool VContainer::UseCapacity(uint64_t toUse) {
	if (CanStore(toUse)) {
		usedCapacity += toUse;
		return true;
	} else return false;
}

std::string VContainer::GetPrint() {
	std::string outputPrint;
	outputPrint +=( "["
		+ GetPath().string() + ","
		+ std::to_string(GetTotalCapacity()) + ","
		+ std::to_string(GetUsedCapacity()) +
		"]");

	return outputPrint;
}
