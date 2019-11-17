#pragma once

#include "ConnectionInfo.h"
#include "FileUploadInfo.h"
#include "VFile.h"

#include <fstream>
#include <vector>

class CommunicationHandler {
public:
	static std::pair<int, std::string> ReceiveMsg(SOCKET& socket);
	static int SendBasicMsg(SOCKET& socket, std::string _msg);

	static void ReceiveFile(SOCKET& socket, VFile& fileStream);
};

