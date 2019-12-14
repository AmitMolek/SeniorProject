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

namespace fs = std::filesystem;
namespace db = dbHandler;
namespace bo = boost;

void DataTransferHandler::Thread_GetData(ConnectionInfo* con, FileUploadInfo fileInfo) {
	std::pair<int, string> msgInfo;

	VFile outFile;
	con->storage->AllocateFiles({ {outFile, fileInfo} });

	ConsoleOutput() << "[INFO][" << con->clientAddress << "] Started handling client file " << outFile << "\n";
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
			VContainer* parentCont = (VContainer*)outFile.GetParent();
			parentCont->UseCapacity(outFile.fileSize);
			parentCont->files.push_back(std::move(outFile));
			break;
		}
	}
	CommunicationHandler::SendBasicMsg(*con->instructionSocket,"|pass:server_file_finish");
	ConsoleOutput() << "[INFO][" << con->clientAddress << "] Stopped handling client file " << fileInfo.fileName << "\n";
}

void DataTransferHandler::Thread_SendListFiles(ConnectionInfo* con) {
	std::vector<std::string> results;
	dbHandler::Database().getListFiles(con->username, results);
	std::stringstream output;

	output << "|pass:send_list_files:";
	for (std::string s : results){
		output << s << ",";
	}

	CommunicationHandler::SendBasicMsg(*con->dataSocket, output.str());
}