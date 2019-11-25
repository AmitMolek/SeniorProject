#include "dbHandler.h"
#include "sqlite3.h"
#include <string>
#include "ConsoleOutput.h"


using namespace std;




namespace fs = std::filesystem;
  bool dbHandler::addFileToDB(string fileName,fs:: path filePath, string userName, fs::path storagePath)
{
	sqlite3* db;
	char* zErrMsg = 0;
	int rc;
	string sql;
	
	
	std::string tmpPath = filePath.string();
		std::string::size_type i = tmpPath.find(storagePath.string());
		if (i != std::string::npos)
		{
			tmpPath.erase(i, storagePath.string().length());
		}
	
		std::replace(tmpPath.begin(), tmpPath.end(), '\\', '/'); // replace all 'x' to 'y'

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

