#include "ConsoleOutput.h"

#include <iostream>
#include <mutex>
#include <ostream>
#include <sstream>

std::mutex ConsoleOutput::printLock{};

ConsoleOutput::~ConsoleOutput() {
	std::lock_guard<std::mutex> guard(printLock);
	std::cout << this->str();
}