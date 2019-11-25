#include "StorageObject.h"

StorageObject::StorageObject(fs::path _rootPath,
							 unsigned long long int _capacity, 
							 unsigned long long int _usedCapacity, 
							 StorageObject * _parent) {
	parentObject = _parent;
	rootPath = _rootPath;
	capacity = _capacity;
	usedCapacity = _usedCapacity;
}

StorageObject::StorageObject(const StorageObject& other) {
	parentObject = other.parentObject;
	rootPath = other.rootPath;
	capacity = other.capacity;
	usedCapacity = other.usedCapacity;
}

StorageObject::StorageObject(StorageObject&& other) noexcept : 
	parentObject(std::exchange(other.parentObject, nullptr)),
	rootPath(std::move(other.rootPath)), 
	capacity(std::exchange(other.capacity, 0)), 
	usedCapacity(std::exchange(other.usedCapacity, 0)){ }

StorageObject* StorageObject::GetParent() {
	return parentObject;
}

fs::path StorageObject::GetPath() {
	return rootPath;
}

unsigned long long int StorageObject::GetTotalCapacity() {
	return capacity;
}

unsigned long long int StorageObject::GetFreeCapacity() {
	return (GetTotalCapacity() - GetUsedCapacity());
}

unsigned long long int StorageObject::GetUsedCapacity() {
	return usedCapacity;
}

void StorageObject::SetParent(StorageObject* _parent) {
	parentObject = _parent;
}

void StorageObject::SetPath(fs::path _path) {
	rootPath = _path;
}

void StorageObject::SetPath(fs::path&& _path) {
	rootPath = std::move(_path);
}

void StorageObject::SetTotalCapacity(unsigned long long int _capacity) {
	capacity = _capacity;
}

void StorageObject::SetUsedCapacity(unsigned long long int _usedCapacity) {
	usedCapacity = _usedCapacity;
}

bool StorageObject::CanStore(unsigned long long int toStore) {
	return (GetTotalCapacity() > (GetUsedCapacity() + toStore)) ? true : false;
}

bool StorageObject::UseCapacity(unsigned long long toUse) {
	if (CanStore(toUse)) {
		usedCapacity += toUse;
		return true;
	} else return false;
}

std::ostream& operator<<(std::ostream& out, StorageObject& obj) {
	out << "[" 
		<< "Parent=" << obj.parentObject 
		<< "," << "Path=" << obj.rootPath
		<< "," << "Capacity=" << obj.capacity
		<< "," << "Used=" << obj.usedCapacity
		<< "," << "Free=" << obj.GetFreeCapacity()
		<< "]";
	return out;
}