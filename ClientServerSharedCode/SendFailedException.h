#pragma once

#include <iostream>
#include <exception>

class SendFailedException : public std::exception{

	std::string msg;
public:
	SendFailedException() = default;
	SendFailedException(std::string _msg);

	const char* what() const throw();

};

