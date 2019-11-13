#pragma once

#include "ConnectionInfo.h"
#include "FileUploadInfo.h"

class DataTransferHandler {
public:

	static void Thread_GetData(ConnectionInfo* con, FileUploadInfo fileInfo);

private:
};

