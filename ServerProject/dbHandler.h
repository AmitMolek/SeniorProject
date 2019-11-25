#include <string>
#include <filesystem>
namespace dbHandler {
	

	bool addFileToDB(std::string fileName, std::filesystem::path filePath,std:: string userName, std::filesystem::path storagePath);
}