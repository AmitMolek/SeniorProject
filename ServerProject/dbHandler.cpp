#include "dbHandler.h"
#include "ConsoleOutput.h"
#include "sqlite3.h"

#include <utility>

using namespace std;
namespace fs = std::filesystem;
namespace dbh = dbHandler;

sqlite3* dbh::Database::db = nullptr;
std::string dbh::Database::name = "";

dbh::Database::Database() {
	if (!dbh::Database::db)
		dbh::Database::OpenDatabase("binPacking.db");
}

dbh::Database& dbHandler::Database::Instance() {
	dbh::Database instance;
	return instance;
}

void dbh::Database::OpenDatabase(std::string dbName){
	int returnCode = -1;

	dbh::Database::name = dbName;

	if ((returnCode = sqlite3_open(dbh::Database::name.c_str(), &dbh::Database::db))) {
		ConsoleOutput() << "[SQL-ERR] " << "Can't open database: " << sqlite3_errmsg(dbh::Database::db) << "\n";
	}
	ConsoleOutput() << "[SQL-INFO] Database " << dbh::Database::name << " is open\n";
}

void dbh::Database::CloseDatabase() {
	if (dbh::Database::db) {
		sqlite3_close(dbh::Database::db);
		ConsoleOutput() << "[SQL-INFO] Database " << dbh::Database::name << "is closed\n";
	}
}

bool dbh::Database::ExecQuery(std::string query){
	int returnCode = -1;
	char* errMsg;

	if ((returnCode = sqlite3_exec(dbh::Database::db, query.c_str(), NULL, 0, &errMsg)) != SQLITE_OK) {
		ConsoleOutput() << "[SQL-ERR] Failed to execute SQL query: " << query << " | " << errMsg << "\n";
		sqlite3_free(errMsg);
		return false;
	}

	ConsoleOutput() << "[SQL-INFO] Executed SQL query: " << query << "\n";
	return true;
}

bool dbh::Database::StoreFile(std::string fileName, 
							 fs::path filePath, 
							 std::string userName, 
							 fs::path storagePath,
							 uint64_t fileSize){
	std::string tmpPath = filePath.parent_path().string();
	std::string::size_type i = tmpPath.find(storagePath.string());
	if (i != std::string::npos) {
		tmpPath.erase(i, storagePath.string().length());
	}

	std::replace(tmpPath.begin(), tmpPath.end(), '\\', '/');

	std::string sql = "INSERT INTO files (fileName,folder,owner,size)"\
		"VALUES ( '" + fileName + "','" + tmpPath + "', '" + userName + "','"+ to_string(fileSize) +"');";

	return dbh::Database::ExecQuery(sql);
}

dbh::Database& dbHandler::Database::operator()() {
	return dbh::Database::Instance();
}

void dbHandler::operator<<(Database& out, const std::string query) {
	out.ExecQuery(query);
}

void dbHandler::operator<<(Database& out, std::pair<VFile*, ConnectionInfo*> uploadInfo) {
	VFile* file = uploadInfo.first;
	ConnectionInfo* con = uploadInfo.second;

	out.StoreFile(file->fileName, file->GetPath(), con->username, con->storage->GetPath(),file->fileSize);
}
/* not used */
bool dbHandler::addFileToDB(string fileName,fs:: path filePath, string userName, fs::path storagePath)
{
	sqlite3* db;
	char* zErrMsg = 0;
	int rc;
	string sql;
	
	
	std::string tmpPath = filePath.parent_path().string();
		std::string::size_type i = tmpPath.find(storagePath.string());
		if (i != std::string::npos)
		{
			tmpPath.erase(i, storagePath.string().length());
		}
	
		std::replace(tmpPath.begin(), tmpPath.end(), '\\', '/');

	/* Open database */
	rc = sqlite3_open("binPacking.db", &db);
	if (rc) {
		std::cout << "Can't open database: %s" << sqlite3_errmsg(db);

	}
	else {
		std::cout << "Opened DB successfully" << std::endl;
	}
	sql = "INSERT INTO files (fileName,folder,owner)"\
		"VALUES ( '" + fileName + "','" + tmpPath +"', '" + userName + "');";	

	rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg);

	if (rc != SQLITE_OK) {
		cout << "SQL error: " << zErrMsg << endl;
		sqlite3_free(zErrMsg);
		return false;
	}
	else {
		cout << "Records created successfully" << endl;
		
	}
	sqlite3_close(db);
	return true;
}

/*operator << for insert new container*/
void dbHandler::operator<<(Database& out, VContainer *container) {
	fs::path path = container->GetPath();
	out.InsertVContainer(path.filename().string(), container->GetTotalCapacity(), container->GetUsedCapacity());
}
bool dbh::Database::InsertVContainer(std::string FolderName,
												uint64_t capacity,
												uint64_t usedCapacity) {
	
	std::string sql = "INSERT INTO containers (Name,capacity,usedCapacity)"\
		"VALUES ( '"+ FolderName +"','" + to_string(capacity) + "', '" + to_string(usedCapacity) + "');";

	return dbh::Database::ExecQuery(sql);
}

int dbHandler::Database::updateVcontainerUsedCapacity(std::string containerName, uint64_t usedAdded)
{
	std::string sql = "UPDATE containers SET usedCapacity = usedCapacity+" + to_string(usedAdded) +" WHERE name = " + containerName + ";";
	int rc = sqlite3_exec(dbh::Database::Instance().db, sql.c_str(), NULL, NULL, NULL);
	//return dbh::Database::ExecQuery(sql);

	return 0;
}

static int retrieveFiles_callback(void* param, int argc, char** argv, char** azColName)
{
	if (argc == 0) return 0;
	//for (int i = 0; i < 4; i++)
	//{
	//	cout << azColName[i] << " " << argv[i] << endl;
	//}

	std::pair<VContainer&, std::vector<VFile>*>* pointerParams = (std::pair<VContainer&, std::vector<VFile>*> *)param;
	std::vector<VFile>* param1 = pointerParams->second;

	fs::path fileRoot = fs::current_path();
	fileRoot /= "VirtualStorage\\";
	fileRoot /= argv[1];
	string fileName = argv[0];
	uint64_t fileSize = atoi(argv[3]);
	uint64_t containerUsedCapacity = atoi(argv[2]);
	VFile tmpVFile(fileRoot, fileName, fileSize, &pointerParams->first);

	(*param1).push_back(std::move(tmpVFile));
	return 0;
}

bool retrieveFiles(VContainer &parent,std::vector<VFile>* files, std::string folderName)
{
	std::pair<VContainer&, std::vector<VFile>* > params = { parent ,files };
	std::pair<VContainer & , std::vector<VFile> * > *pointerParams = &params;
	//std::string sql = "SELECT * FROM files WHERE folder = " + folderName + ";";
	std::string sql = "SELECT * FROM files WHERE folder ='" + folderName + "';";
	int rc = sqlite3_exec(dbh::Database::Instance().db, sql.c_str(), retrieveFiles_callback, pointerParams, NULL);
	
	return 0;
}





static int retrieveContainers_callback(void* param, int argc, char** argv, char** azColName)
{
	if (argc == 0) return 0;
	//for (int i = 0; i < 4; i++) 
	//{
	//	cout << azColName[i] << " " << argv[i]<<endl;
	//}
	std::vector<VContainer>* param1 = (std::vector<VContainer>*)param;
	
	fs::path containerRoot = fs::current_path();
	containerRoot /= "VirtualStorage\\";
	containerRoot /= argv[1];
	uint64_t containerCapacity = atoi(argv[2]);
	uint64_t containerUsedCapacity = atoi(argv[3]);
	VContainer tmpContainer(containerRoot, containerCapacity, containerUsedCapacity,NULL);
	string s =  "/";
	s += argv[1];
	
	retrieveFiles(tmpContainer,&tmpContainer.files,s);
	(*param1).push_back(std::move(tmpContainer));
	return 0;
}

bool dbh::Database::retrieveContainers(std::vector<VContainer>& containersVector)
{
	std::string sql = "SELECT * FROM containers;";
	int rc = sqlite3_exec(dbh::Database::Instance().db, sql.c_str(), retrieveContainers_callback, &containersVector, NULL);

	return true;
}

 void dbh::Database::setCurrentAlgorithm(std::string algoName)
{
	std::string sql = "UPDATE generalData SET currentAlgo = ' " + algoName + "';";
		
	int rc = sqlite3_exec(dbh::Database::Instance().db, sql.c_str(), NULL, NULL, NULL);

	
}

static int getNumOfContainers_callback(void* param, int argc, char** argv, char** azColName)
{
	if (argc == 0) return 0;
	
	int* count = (int*)param;
	*count = atoi(argv[0]);

	return 1;
}

static int getListFiles_callBack(void* param, int argc, char** argv, char** azColName){
	std::vector<std::string>* results = (std::vector<std::string>*)param;
	std::stringstream ssOut;
	ssOut << azColName[0];
	ssOut << ":";
	ssOut << argv[0];
	ssOut << "^";
	ssOut << azColName[1];
	ssOut << ":";
	ssOut << argv[1];
	results->push_back(ssOut.str());
	return 0;
}

bool dbh::Database::getListFiles(std::string username, std::vector<std::string>& results) {
	std::string sql = "SELECT fileName,size FROM files WHERE owner='" + username + "';";
	int rc = sqlite3_exec(dbh::Database::Instance().db, sql.c_str(), getListFiles_callBack, &results, NULL);

	return true;
}

bool dbh::Database::getNumOfContainers(unsigned int *count)
{
	
	std::string sql = "SELECT COUNT(id) FROM containers;";
	int rc = sqlite3_exec(dbh::Database::Instance().db, sql.c_str(), getNumOfContainers_callback, count, NULL);

	return true;
}

