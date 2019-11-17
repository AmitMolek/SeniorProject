#pragma once

#include "VFile.h"

#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

class VFolder {
public:
	fs::path rootPath;
	std::size_t capacity;
	//std::vector<VFile> files;
};

