#pragma once
#include <exception>
#include <iostream>

#include "FatalException.h"

/**
* @brief InputException raised if input did not go as excepted
*/
class InputException final /*final = cannot be inherent*/ : public std::exception {
public:
	InputException(const std::string& msg) : std::exception(), _msg(msg) {};
	virtual ~InputException() noexcept = default;

	virtual const char* what() const noexcept { return this->_msg.c_str(); }

private:
	std::string _msg;
};