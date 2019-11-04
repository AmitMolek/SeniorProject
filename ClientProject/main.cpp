#include <iostream>
#include <fstream>

#include <filesystem>

//#include "ClientSocket.h"
#include "BaseClient.h"
#include "SocketHelper.h"

using namespace std;
namespace fs = std::filesystem;

void testReadFile() {
	fs::path projectPath = fs::current_path();
	fs::path testFolder = "TestFiles";

	fs::path testPath = projectPath;
	fs::path testFilePath;
	testPath /= testFolder;
	testFilePath = testPath;
	testFilePath /= "testFile.jpg";

	cout << "Test file path: " << testFilePath << '\n';

	ifstream testFile(testFilePath, std::ios::binary);

	if (!testFile) {
		cerr << "Test file could not be open!" << '\n';
		exit(EXIT_FAILURE);
	}

	cout << "Test file is open!" << '\n';

	const int bufferSize = 1024;
	char buffer[bufferSize];

	streampos begSeek = testFile.tellg();
	testFile.seekg(0, std::ios::end);
	streampos endSeek = testFile.tellg();
	streampos fileSize = endSeek - begSeek;

	cout << "File size = " << fileSize << '\n';
	cout << "Loops = " << fileSize / 1024 << '\n';

	testFile.seekg(0, std::ios::beg);

	fs::path outputFilePath = testPath;
	outputFilePath /= "output.jpg";

	ofstream outputFile(outputFilePath, std::ios::binary);
	if (!outputFile) {
		cerr << "Output file could not be open!" << '\n';
		exit(EXIT_FAILURE);
	}

	while (!testFile.eof()){
		testFile.read(buffer, bufferSize);
		//outputFile << buffer;
		outputFile.write(buffer, bufferSize);
	}

	testFile.close();
	outputFile.close();
}

int main(){
	//ClientSocket::InitWinsock();
	//ClientSocket cs("10.0.0.100");
	//ClientSocket cs;
	
	//testReadFile();

	SocketHelper::InitWinSock();
	BaseClient bc{"10.0.0.29", "23456", "23457"};
}