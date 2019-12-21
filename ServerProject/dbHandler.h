#include "sqlite3.h"
#include "VFile.h"
#include "ConnectionInfo.h"

#include <string>
#include <filesystem>
#include <utility>
#include <vector>

namespace fs = std::filesystem;

namespace dbHandler {
	
	class Database {
	public:
		static sqlite3* db;
		static std::string name;

		Database();
		Database(const Database& _db) = delete;
		Database(Database&& _db) = delete;

		static Database& Instance();

		static void OpenDatabase(std::string dbName);
		static void CloseDatabase();
		static bool ExecQuery(std::string query);

		static bool StoreFile(std::string fileName, fs::path filePath, std::string userName, fs::path storagePath, uint64_t fileSize);

		bool InsertVContainer(std::string FolderName, uint64_t capacity, uint64_t usedCapacity);

		int  updateVcontainerUsedCapacity(std::string containerName, uint64_t usedAdded);

		Database& operator () ();

		friend void operator << (Database& out, const std::string query);
		friend void operator << (Database& out, std::pair<VFile*, ConnectionInfo*> uploadInfo);
		//bool retrieveFiles(std::vector<VFile>& files, std::string folderName);
		bool retrieveContainers(std::vector<VContainer>& containersVector);
		void setCurrentAlgorithm(std::string containersVector);
		bool getNumOfContainers(unsigned int* count);
		bool retrieveFiles(VContainer& parent, std::vector<VFile>* files, std::string folderName);
		bool getListFiles(std::string username, std::vector<std::string>& results);
		bool getUserFilesPath(std::string username, std::vector<std::string>& results);
	};
	bool addFileToDB(std::string fileName, fs::path filePath,std:: string userName, fs::path storagePath);
	void operator<<(Database& out, VContainer* container);
}