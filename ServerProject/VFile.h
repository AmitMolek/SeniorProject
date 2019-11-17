#pragma once

#include <fstream>
#include <sstream>
#include <ostream>
#include <filesystem>

#include <iostream>

namespace fs = std::filesystem;

class VFile {
public:
	fs::path rootPath;
	std::ofstream fileStream;

	std::string fileName;
	unsigned long long int fileSize;

	std::stringstream fileBuffer;

	VFile();
	VFile(const VFile& other) = delete;
	VFile(VFile&& other) noexcept;
	VFile(fs::path _rootPath, std::string _fileName, unsigned long long int _fileSize);

	VFile& operator << (std::string _str);
	friend std::ostream& operator << (std::ostream& out, const VFile& obj);

	void operator = (VFile&& _vFile) noexcept;

	void OpenFileStream();
	void CloseFileStream();
	void SaveFileStream();

	void AppendBytes(std::string _str);
};

