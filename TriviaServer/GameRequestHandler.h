#pragma once
#include "IRequestHandler.h"
#include "NotifyRoomChanges.h"
#include "EnumCodes.h"
#include <chrono>

class RequestHandlerFactory;

class GameRequestHandler : public IRequestHandler
{
public:
	using HandleMethod = RequestResult(GameRequestHandler::*)(const RequestInfo&, ClientData&);

	GameRequestHandler();
	GameRequestHandler(const int gameID);

	/**
	* @brief checks if the actions is valid
	* @param reqInfo the buffer got
	* @param data the username and handler of the user
	* @throw ClientConnectionException if the request is not relevant
	*/
	virtual void isRequestRelevant(const RequestInfo& reqInfo, ClientData& data) override; //throw ClientConnectionException

	/**
	* @brief handle the request and send a response
	* @param reqInfo the buffer got
	* @param data the username and handler of the user
	* @returns request result with new handler and buffer to send to the client
	*/
	virtual RequestResult handleRequest(const RequestInfo& reqInfo, ClientData& data) override;

private:
	/**
	* @brief handle the leave request from the room
	* @param reqInfo the buffer got
	* @param data the username and handler of the user
	* @returns request result with new handler and buffer to send to the client
	*/
	RequestResult handleLeaveGameRequest(const RequestInfo& reqInfo, ClientData& data);

	/**
	* @brief handle the getting the next question and its answers
	* @param reqInfo the buffer got
	* @param data the username and handler of the user
	* @param request result with new handler and buffer to send to the client
	*/
	RequestResult handleGetQuestionRequest(const RequestInfo& reqInfo, ClientData& data);

	/**
	* @brief handle the submit of the answer
	* @param reqInfo the buffer got
	* @param data the username and handler of the user
	* @returns request result with new handler and buffer to send to the client
	*/
	RequestResult handleSubmitAnswerRequest(const RequestInfo& reqInfo, ClientData& data);

	/**
	* @brief handle to send the user the game (data of the game) 
	* @param reqInfo the buffer got
	* @param data the username and handler of the user
	* @returns request result with new handler and buffer to send to the client
	*/
	RequestResult handleGetGameResultRequest(const RequestInfo& reqInfo, ClientData& data);

	/**
	* @brief handle the notification of close room
	* @param reqInfo the buffer got
	* @param data the username and handler of the user
	* @returns request result with new handler and buffer to send to the client
	*/
	RequestResult handleNotifyCloseGameRequest(const RequestInfo& reqInfo, ClientData& /*NOT IN USE*/);

	/**
	* @brief handler the notification of member left
	* @param reqInfo the buffer got
	* @param data the username and handler of the user
	* @returns request result with new handler and buffer to send to the client
	*/
	RequestResult handleNotifyAdminMemberLeft(const RequestInfo& reqInfo, ClientData& /*NOT IN USE*/);

	std::unique_ptr<RequestHandlerFactory> _factory;

	int _questionsAnswered;
	const int _gameID;

	std::chrono::steady_clock::time_point _creationTime; //will be assigned in c'tor - just after the sent of the last mag

	static const std::vector<ClientCodes> _validCodes;

	static const std::map<ClientCodes, HandleMethod> _actions;
};

