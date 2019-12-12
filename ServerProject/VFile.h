#pragma once

#include "StorageObject.h"

#include <fstream>
#include <sstream>
#include <ostream>
#include <filesystem>

#include <iostream>

namespace fs = std::filesystem;

class VFile : public StorageObject{
public:
	std::ofstream fileStream;
	std::string fileName;
	uint64_t fileSize;
	std::stringstream fileBuffer;

	VFile();
	VFile(const VFile& other) = delete;
	VFile(VFile&& other) noexcept;
	VFile(fs::path _rootPath, std::string _fileName, 
		  uint64_t _fileSize, StorageObject* _parent = nullptr);

	VFile& operator << (std::string _str);
	friend std::ostream& operator << (std::ostream& out, const VFile& obj);

	void operator = (VFile&& _vFile) noexcept;

	void OpenFileStream();
	void CloseFileStream();
	void SaveFileStream();

	void AppendBytes(std::string _str);
};

