#pragma once

#include <iostream>

class FileUploadInfo {
public:

	std::string fileName;
	int fileSize;

	FileUploadInfo(std::string _fileName, int _fileSize);
};

