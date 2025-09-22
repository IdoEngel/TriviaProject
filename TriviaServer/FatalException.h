#pragma once
#include <string>
#include <exception>

/**
* @brief fatal exception is an exception that the program cannot continue if happened
*/
class FatalException final /*final = cannot be inherent*/ : public std::exception {
public:
	FatalException(const std::string msg) noexcept : _msg(msg) {}
	virtual ~FatalException() noexcept = default;

	virtual const char* what() const noexcept { return _msg.c_str(); }

private:
	std::string _msg;
};
