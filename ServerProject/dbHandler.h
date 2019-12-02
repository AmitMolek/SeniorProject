#include "sqlite3.h"
#include "VFile.h"
#include "ConnectionInfo.h"

#include <string>
#include <filesystem>
#include <utility>

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

		static bool StoreFile(std::string fileName, fs::path filePath, std::string userName, fs::path storagePath);

		bool InsertVContainer(std::string FolderName, int capacity, int usedCapacity);

		int  updateVcontainerUsedCapacity(std::string containerName,int usedAdded);

		Database& operator () ();

		friend void operator << (Database& out, const std::string query);
		friend void operator << (Database& out, std::pair<VFile*, ConnectionInfo*> uploadInfo);
	};
	bool addFileToDB(std::string fileName, std::filesystem::path filePath,std:: string userName, std::filesystem::path storagePath);
	void operator<<(Database& out, VContainer* container);
}