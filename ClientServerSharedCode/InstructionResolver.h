#pragma once

#include <iostream>
#include <vector>

class Instruction {
public:
	std::string status;
	std::string code;
	std::vector<std::string> content;

	Instruction();

	friend std::ostream& operator << (std::ostream& out, const Instruction& data);
};

class InstructionResolver {
public:
	static std::vector<Instruction> Resolve(std::string msg);

};

