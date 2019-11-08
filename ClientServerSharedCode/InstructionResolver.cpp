#include "InstructionResolver.h"

#include <iostream>
#include <vector>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

using namespace std;

Instruction::Instruction() : status(), code(), content(){}

std::ostream& operator<<(std::ostream& out, const Instruction& data) {
	int i = 1;
	out << "(";
	out << data.status;
	out << ":";
	out << data.code;
	out << ":";

	for (string inst : data.content) {
		if (i != data.content.size())
			out << inst << ",";
		else out << inst;
		i++;
	}

	out << ")";
	return out;
}

vector<Instruction> InstructionResolver::Resolve(string msg){
	vector<Instruction> instructions;
	vector<string> splitMsg;
	vector<string> splitInstruction;

	boost::split(splitMsg, msg, boost::is_any_of("|"));
	for(string instruction : splitMsg){
		boost::split(splitInstruction, instruction, boost::is_any_of(":"));
		if (splitInstruction.size() > 1) {
			Instruction inst;
			inst.status = splitInstruction[0];
			inst.code = splitInstruction[1];
			if (splitInstruction.size() > 2)
				boost::split(inst.content, splitInstruction[2], boost::is_any_of(","));

			instructions.push_back(inst);
		}
	}

	return instructions;
}
