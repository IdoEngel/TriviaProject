#pragma once
#include <string>
#include <exception>

/**
* @brief DB_Exception is an exception that related to DB access and manage
*/
class DB_Exception : public std::exception {
public:
	DB_Exception(const std::string msg) noexcept : _msg(msg) {}
	virtual ~DB_Exception() noexcept = default;

	virtual const char* what() const noexcept { return _msg.c_str(); }

private:
	std::string _msg;
};
