#pragma once

#include <filesystem>

namespace fs = std::filesystem;

class StorageObject {
protected:
	StorageObject* parentObject;
	fs::path rootPath;

public:
	StorageObject(fs::path _rootPath, StorageObject * _parent = nullptr);
	StorageObject(const StorageObject& other);
	StorageObject(StorageObject&& other) noexcept;

	StorageObject* GetParent();
	fs::path GetPath();

	void SetParent(StorageObject* _parent);
	void SetPath(fs::path _path);
	void SetPath(fs::path&& path);

	friend std::ostream& operator << (std::ostream& out, StorageObject& obj);
};

