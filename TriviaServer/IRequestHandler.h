#pragma once
#include <memory>
#include <ctime>
#include <vector>
#include <cstddef>
#include "RequestResponseStructs.h"
#include <optional>

class IRequestHandler;

/**
* @brief the result of the request
* @brief buffer : Buffer
* @brief newHandler : IRequestHandler*
*/
typedef struct RequestResult {
	Buffer buffer; //buffer of info
	std::shared_ptr<IRequestHandler> newHandler; //handler of the result
} RequestResult;

/**
* @brief data of a client communicating with the server
* @brief _handler : std::shared_ptr<IRequestHandler>
* @brief _username : std::optional<std::string>
*/
typedef struct ClientData {
	ClientData() = default;
	ClientData(const std::optional<std::string>& username, std::shared_ptr<IRequestHandler> handler) {
		this->_handler = handler;
		this->_username = username;
	}
	std::shared_ptr<IRequestHandler> _handler;
	std::optional<std::string> _username;
} ClientData;

/**
* @brief INTERFACE - only virtual funcs
* @brief functions all the classes of type Handler will need to create
* @note the classes that will inherent from this will be able to create more then the funcs in here
*/
class IRequestHandler {
public:
	virtual void isRequestRelevant(const RequestInfo& reqInfo, ClientData& data) = 0; //throw ClientConnectionException
	virtual RequestResult handleRequest(const RequestInfo& reqInfo, ClientData& data) = 0;
};

