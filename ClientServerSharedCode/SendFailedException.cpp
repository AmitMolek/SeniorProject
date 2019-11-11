#include "SendFailedException.h"

SendFailedException::SendFailedException(std::string _msg){
	msg = _msg;
}

const char* SendFailedException::what() const throw(){
	return ("Failed sending " + msg).c_str();
}