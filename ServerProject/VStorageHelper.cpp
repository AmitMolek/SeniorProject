#include "VStorageHelper.h"

#include <iostream>
#include <string>

std::vector<fs::path> VStorageHelper::GetSerialPaths(fs::path root, unsigned int count){
	std::vector<fs::path> paths;

	fs::path tmpRoot = root;
	for (unsigned int i = 0; i < count; i++) {
		fs::path serialPath = tmpRoot;
		serialPath /= std::to_string(i);
		paths.push_back(serialPath);
	}

	return paths;
}