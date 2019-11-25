#include "InstructionHandler.h"
#include "ConsoleOutput.h"
#include "CommunicationHandler.h"
#include "DataTransferHandler.h"
#include "VFile.h"
#include "VStorage.h"

#include <iostream>
#include <WinSock2.h>
#include <string>
#include <utility>
#include <thread>

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
	}else if (ins.code == "file_send"){
		InstructionHandler::Handle_Pass_file_send(ins, con);
	}
	else if ((ins.code == "user_name"))
	{
		InstructionHandler::Handle_Pass_user_name(ins, con);
	}
	
}

void InstructionHandler::Handle_Fail(Instruction ins, ConnectionInfo& con){
	
}

void InstructionHandler::Handle_Pass_data_socket_id(Instruction ins, ConnectionInfo& con) {
	*con.dataSocket = stoi(ins.content[0]);
	ConsoleOutput() << "[INFO][" << con.clientAddress << "] Got data socket " << *con.dataSocket << "\n";

	CommunicationHandler::SendBasicMsg(*con.instructionSocket, "|pass:socket_id");
}

void InstructionHandler::Handle_Pass_file_send(Instruction ins, ConnectionInfo& con) {
	string fileName = ins.content[0];
	int fileSize = stoi(ins.content[1]);
	FileUploadInfo fileInfo(fileName, fileSize);

	//VFile outFile = con.storage->GetFile(fileInfo);

	std::thread dataTransferThread(DataTransferHandler::Thread_GetData, &con, fileInfo);
	dataTransferThread.detach();
}
void InstructionHandler::Handle_Pass_user_name(Instruction ins, ConnectionInfo& con) {
	string userName = ins.content[0];
	con.username = userName;

	
}