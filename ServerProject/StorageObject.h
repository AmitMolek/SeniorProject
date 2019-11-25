#pragma once

#include <filesystem>

namespace fs = std::filesystem;

class StorageObject {
private:
	StorageObject* parentObject;

	fs::path rootPath;
	unsigned long long int capacity;
	unsigned long long int usedCapacity;

public:
	StorageObject(fs::path _rootPath, 
				  unsigned long long int _capacity, 
				  unsigned long long int _usedCapacity = 0, 
				  StorageObject* _parent = nullptr);
	StorageObject(const StorageObject& other);
	StorageObject(StorageObject&& other) noexcept;

	StorageObject* GetParent();
	fs::path GetPath();
	unsigned long long int GetTotalCapacity();
	unsigned long long int GetFreeCapacity();
	unsigned long long int GetUsedCapacity();

	void SetParent(StorageObject* _parent);
	void SetPath(fs::path _path);
	void SetPath(fs::path&& path);
	void SetTotalCapacity(unsigned long long int _capacity);
	void SetUsedCapacity(unsigned long long int _usedCapacity);

	bool CanStore(unsigned long long int toStore);
	bool UseCapacity(unsigned long long toUse);

	friend std::ostream& operator << (std::ostream& out, StorageObject& obj);
};

