#pragma once

#include "ConnectionInfo.h"
#include "InstructionResolver.h"

class InstructionHandler {
public:
	static void HandleInstruction(Instruction ins, ConnectionInfo& con);

private:
	static void Handle_Pass(Instruction ins, ConnectionInfo& con);
	static void Handle_Fail(Instruction ins, ConnectionInfo& con);

	static void Handle_Pass_data_socket_id(Instruction ins, ConnectionInfo& con);
	static void Handle_Pass_file_send(Instruction ins, ConnectionInfo& con);
	static void Handle_Pass_user_name(Instruction ins, ConnectionInfo& con);
	static void Handle_Pass_Get_List_Files(Instruction ins, ConnectionInfo& con);
	static void Handle_Pass_file_get(Instruction ins, ConnectionInfo& con);
};

