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
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

using namespace std;

namespace fs = std::filesystem;
namespace db = dbHandler;
namespace bo = boost;

void DataTransferHandler::Thread_GetData(ConnectionInfo* con, FileUploadInfo fileInfo) {
	std::pair<int, string> msgInfo;

	VFile outFile;
	con->storage->AllocateFiles({ {outFile, fileInfo} });

	if (con) {
		ConsoleOutput() << "[INFO][" << con->clientAddress << "] Started handling client file " << outFile << "\n";
	} else {
		return;
	}
	CommunicationHandler::SendBasicMsg(*con->instructionSocket, "|pass:server_wait_on_file");

	string fileStartInstruction = "|pass:file_start";
	string fileEndInstruction = "|pass:file_end";

	bool isSendStart = false;
	bool isSendEnd = false;

	if (fileInfo.fileName == "160.txt"){
		ConsoleOutput() << "" << std::endl;
	}

	while ((msgInfo = CommunicationHandler::ReceiveMsg(*con->dataSocket)).first != -1){
		//ConsoleOutput() << msgInfo.second << "\n";
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
			//ConsoleOutput() << "Found file end!" << std::endl;
			if (fileStartPos != std::string::npos)
				msg.erase(fileEndPos - fileStartInstruction.size(), fileEndInstruction.size());
			else msg.erase(fileEndPos, fileEndInstruction.size());
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

bool SkipBOM(std::istream& in) {
	char test[4] = { 0 };
	in.read(test, 3);
	if (strcmp(test, "\xEF\xBB\xBF") == 0)
		return true;
	in.seekg(0);
	return false;
}

void DataTransferHandler::Thread_SendFiles(ConnectionInfo* con, std::vector<std::string> filesNames) {
	std::vector<std::string> filesInfo;
	std::vector<pair<std::string, fs::path>> finalFileInfo;
	db::Database().Instance().getUserFilesPath(con->username, filesInfo);

	for (std::string fileInfo : filesInfo){
		std::vector<std::string> difInfo;
		boost::split(difInfo, fileInfo, boost::is_any_of("^"));
		if (difInfo.size() > 0){
			fs::path filePath = con->storage->GetPath();
			filePath /= fs::path(difInfo[1]).filename();
			filePath /= difInfo[0];
			finalFileInfo.push_back(make_pair(difInfo[0], filePath));
		}
	}

	std::vector<pair<std::string, fs::path>> toSend;
	for (std::string s : filesNames){
		for (auto pair : finalFileInfo){
			if (s == pair.first)
				toSend.push_back(pair);
		}
	}

	for (auto pair : toSend){
		std::stringstream finalMsg;
		std::stringstream fileBuffer;
		size_t fileSize;

		std::string prefixFile = "|pass:server_file_start:" + pair.first + ":";
		std::string postfixFile = "|pass:server_file_end";

		std::ifstream fileStream(pair.second, std::ios::binary);

		fileStream.seekg(0, fileStream.end);
		fileSize = fileStream.tellg();
		fileStream.seekg(0, fileStream.beg);

		fileBuffer << fileStream.rdbuf();
		fileStream.close();

		finalMsg << prefixFile;
		finalMsg << fileBuffer.str();
		finalMsg << postfixFile;

		size_t finalMsgSize = prefixFile.size() + postfixFile.size() + fileSize;

		//ConsoleOutput() << "File size: " << fileSize << " | Msg size: " << finalMsgSize << std::endl;

		CommunicationHandler::SendAllMsg(*con->dataSocket, finalMsg.str().c_str(), finalMsgSize);

		ConsoleOutput() << "[INFO][" << con->clientAddress << "]" << " Sent file " << pair.first << " to client" << std::endl;
	}
}