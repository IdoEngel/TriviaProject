#pragma once
#include <regex>
#include "json.hpp"
#include "IRequestHandler.h"
#include "EnumCodes.h"
#include "RequestResponseStructs.h"
#include "JsonRequestPacketDeserializer.h"
#include "JsonResponsePacketSerializer.h"
#include "MenuRequestHandler.h"
#include "LoginManager.h"
#include "RequestHandlerFactory.h"


/**
* @brief deals with sign-up and log-in requests
*/
class LoginRequestHandler : public IRequestHandler
{
public:
	LoginRequestHandler();

	/**
	* @brief advanced and deep checks - like DB checks, email format etc.
	* @param reqInfo the info of the request - including the full buffer
	* @param data the client's handler and username
	* @throw ClientConnectionException if the request is NOT relevant
	*/
	virtual void isRequestRelevant(const RequestInfo& reqInfo, ClientData& date) override;

	/**
	* @brief handle and do actions on the server according to the request
	* @param reqInfo the information of the request - including the full buffer
	* @param cliData the iterator to update the client name in it
	* @returns the actions result
	*/
	virtual RequestResult handleRequest(const RequestInfo& reqInfo, ClientData& cliData) override;
private:
	/**
	* @brief do the log in and return the result 
	* @param reqInfo the request from the client after built by the server
	* @param cliData the iterator to update the client name in it
	* @returns the result for the server to process
	*/
	RequestResult login(const RequestInfo& reqInfo, ClientData& cliData);

	/**
	* @brief do the sign up and return the result
	* @param reqInfo the request from the client after built by the server
	* @param cliData the iterator to update the client name in it
	* @returns the result for the server to process
	*/
	RequestResult signup(const RequestInfo& reqInfo, ClientData& cliData);

	LoginManager& _manager;
	IDatabase& _db;
};

