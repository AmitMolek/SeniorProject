#pragma once

#include "VStorage.h"

#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

class VStorageHelper {
public:
	static std::vector<fs::path> GetSerialPaths(fs::path root, unsigned int count);
	static void SaveVStorageInfo(VStorage& storage);
};

