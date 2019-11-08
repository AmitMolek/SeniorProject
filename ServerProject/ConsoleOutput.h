#pragma once

#include <iostream>
#include <mutex>
#include <ostream>
#include <sstream>

class ConsoleOutput : public std::ostringstream {
public:
	ConsoleOutput() = default;

	~ConsoleOutput();

private:
	static std::mutex printLock;

};



