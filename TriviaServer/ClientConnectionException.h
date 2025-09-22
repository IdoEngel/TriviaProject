#pragma once
#include <exception>
#include <string>

class ClientConnectionException : public std::exception {
public:


	typedef struct info_t {
		std::string clientMsg;
		std::string serverPrint;
	} ClientError;

	ClientConnectionException(const info_t& msg) : std::exception() {
		this->_msg.clientMsg = msg.clientMsg;
		this->_msg.serverPrint = msg.serverPrint;
	};
	virtual ~ClientConnectionException() = default;

	//the msg for the client
	const std::string client() const noexcept {
		return this->_msg.clientMsg;
	}

	//the msg for the server
	const std::string server() const noexcept {
		return this->_msg.serverPrint;
	}
private:
	info_t _msg;
};
