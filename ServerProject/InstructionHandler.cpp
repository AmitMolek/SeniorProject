#include "InstructionHandler.h"
#include "ConsoleOutput.h"
#include "CommunicationHandler.h"

#include <iostream>
#include <WinSock2.h>
#include <string>
#include <utility>

using namespace std;

void InstructionHandler::HandleInstruction(Instruction ins, ConnectionInfo& con){
	if (ins.status == "pass") {
		InstructionHandler::Handle_Pass(ins, con);
	} else if (ins.status == "fail") {
		InstructionHandler::Handle_Fail(ins, con);
	}
}

void InstructionHandler::Handle_Pass(Instruction ins, ConnectionInfo& con){
	if (ins.code == "data_socket_id") {
		InstructionHandler::Handle_Pass_data_socket_id(ins ,con);
	}
}

void InstructionHandler::Handle_Fail(Instruction ins, ConnectionInfo& con){
	
}

void InstructionHandler::Handle_Pass_data_socket_id(Instruction ins, ConnectionInfo& con) {
	*con.dataSocket = stoi(ins.content[0]);
	ConsoleOutput() << "[INFO][" << con.clientAddress << "] Got data socket " << *con.dataSocket << "\n";
}

void InstructionHandler::Handle_Pass_file_send(Instruction ins, ConnectionInfo& con) {
	string fileName = ins.content[0];
	int fileSize = stoi(ins.content[1]);
	FileUploadInfo info = FileUploadInfo(fileName, fileSize);
	ofstream outFile;
	CommunicationHandler::ReceiveFile(*con.dataSocket, outFile, info);
}