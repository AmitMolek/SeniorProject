#include "DataTransferHandler.h"

#include "CommunicationHandler.h"
#include "ConsoleOutput.h"

#include <utility>
#include <filesystem>
#include <fstream>
#include <string>
#include <boost/algorithm/string.hpp>

using namespace std;

namespace fs = filesystem;

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

	ConsoleOutput() << "[INFO][" << con->clientAddress << "] Started handling client file " << fileInfo.fileName << "\n";
	CommunicationHandler::SendBasicMsg(*con->instructionSocket, "|pass:server_wait_on_file");
	
	ofstream outFile(testFilePath, std::ios::binary);

	bool isEnd = false;
	while ((msgInfo = CommunicationHandler::ReceiveMsg(*con->dataSocket)).first != -1){
		string msg = msgInfo.second;
		//ConsoleOutput() << "[INFO][" << con->clientAddress << "] Sent data: " << msg << "\n";
		if (boost::contains(msg, "|pass:file_start")) {
			string test = "|pass:file_start";
			auto pos = msg.find(test);
			msg.erase(pos, test.size());
			cout << "FOUND START\n";
		}
		if (boost::contains(msg, "|pass:file_end")) {
			string test = "|pass:file_end";
			auto pos = msg.find(test);
			msg.erase(pos, test.size());
			cout << "FOUND END\n";
			isEnd = true;
		}
		outFile.write(msg.c_str(), msg.size());
		if (isEnd) {
			outFile.close();
			cout << "CLOSED FILE\n";
			break;
		}
	}
}
