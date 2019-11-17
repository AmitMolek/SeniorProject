#pragma once

#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

class VStorageHelper {
public:
	static std::vector<fs::path> GetSerialPaths(fs::path root, unsigned int count);
};

