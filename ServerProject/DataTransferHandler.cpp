#include "DataTransferHandler.h"

#include "CommunicationHandler.h"
#include "ConsoleOutput.h"

#include <utility>
#include <filesystem>
#include <fstream>
#include <string>
#include <boost/algorithm/string.hpp>

#include <sstream>
#include "dbHandler.h"


#include <boost/algorithm/string/replace.hpp>
using namespace std;

namespace fs = filesystem;
namespace db = dbHandler;
namespace bo = boost;
void DataTransferHandler::Thread_GetData(ConnectionInfo* con, FileUploadInfo fileInfo) {
	std::pair<int, string> msgInfo;

	fs::path projectPath = fs::current_path();
	fs::path testFolder = "TestFiles";

	fs::path testPath = projectPath;
	fs::path testFilePath;
	testPath /= testFolder;
	testFilePath = testPath;
	fs::create_directories(testFilePath);
	testFilePath /= fileInfo.fileName;


	VFile outFile;
	con->storage->AllocateFile(outFile, fileInfo);

	ConsoleOutput() << outFile << "\n" ;

	ConsoleOutput() << "[INFO][" << con->clientAddress << "] Started handling client file " << fileInfo.fileName << "\n";
	CommunicationHandler::SendBasicMsg(*con->instructionSocket, "|pass:server_wait_on_file");

	string fileStartInstruction = "|pass:file_start";
	string fileEndInstruction = "|pass:file_end";

	bool isSendStart = false;
	bool isSendEnd = false;

	while ((msgInfo = CommunicationHandler::ReceiveMsg(*con->dataSocket)).first != -1){
		string msg = msgInfo.second;
		size_t fileStartPos = msg.find(fileStartInstruction);
		size_t fileEndPos = msg.find(fileEndInstruction);

		if (fileStartPos != std::string::npos) {
			msg.erase(fileStartPos, fileStartInstruction.size());
			ConsoleOutput() << "[INFO][" << con->clientAddress << "] Start receiving file " << fileInfo.fileName << "\n";
			isSendStart = true;
			outFile.OpenFileStream();
		}
		if (fileEndPos != std::string::npos) {
			msg.erase(fileEndPos, fileEndInstruction.size());
			ConsoleOutput() << "[INFO][" << con->clientAddress << "] Stopped receiving file " << fileInfo.fileName << "\n";
			isSendEnd = true;
		}

		if (isSendStart)
			outFile << msg;

		if (isSendEnd) {
			outFile.CloseFileStream();
			db::Database::Instance() << make_pair(&outFile, con);
			ConsoleOutput() << "[INFO][" << con->clientAddress << "] Closed file " << fileInfo.fileName << "\n";
			
			fs::path parentPath = outFile.GetPath().parent_path();
			fs::path VContainerName = parentPath.filename();
			string st = VContainerName.string();
			
			db::Database::Instance().updateVcontainerUsedCapacity(st, outFile.fileSize);
			break;
		}
	}

	ConsoleOutput() << "[INFO][" << con->clientAddress << "] Stopped handling client file " << fileInfo.fileName << "\n";
}
