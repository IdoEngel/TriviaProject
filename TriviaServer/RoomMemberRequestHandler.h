#pragma once
#include "IRequestHandler.h"
#include "NotifyRoomChanges.h"
#include "EnumCodes.h"
#include <map>

class RequestHandlerFactory;

/**
* @brief handler for room members (one who joined it)
*/
class RoomMemberRequestHandler : public IRequestHandler
{
public:	
	using CheckMathod = void(RoomMemberRequestHandler::*)(const RequestInfo&, ClientData&);
	using HandleMethod = RequestResult(RoomMemberRequestHandler::*)(const RequestInfo&, ClientData&);

	RoomMemberRequestHandler();
	virtual ~RoomMemberRequestHandler() = default;

	/**
	* @brief check if the request is relevant and can run it
	* @param reqInfo the buffer request from the client
	* @param data the client's handler and user name
	* @throw ClientConnectionException if the request is not relevant
	*/
	virtual void isRequestRelevant(const RequestInfo& reqInfo, ClientData& data) override; //throw ClientConnectionException

	/**
	* @brief handle the request (do the action in the server)
	* @param reqInfo the buffer request from the client
	* @param data the client's handler and username
	* @return result struct (including buffer)
	*/
	virtual RequestResult handleRequest(const RequestInfo& reqInfo, ClientData& data) override;

private:
	/**
	* @brief make the user leave the room, as requested
	* @param reqInfo the buffer request from the client
	* @param data the client's handler and username
	* @return result struct (including buffer)
	*/
	RequestResult handleLeaveRoom(const RequestInfo& reqInfo, ClientData& data);

	/**
	* @brief get the room state (the curr room of the user)
	* @param reqInfo the buffer request from the client
	* @param data the client's handler and username
	* @return result struct (including buffer)
	*/
	RequestResult handleGetRoomState(const RequestInfo& reqInfo, ClientData& data);

	/**
	* @brief notify the member that the room closed
	* @param reqInfo the buffer request from the client
	* @param data the client's handler and username
	* @return result struct (including buffer)
	*/
	RequestResult handleNotifyCloseRoom(const RequestInfo& reqInfo, ClientData& data);

	/**
	* @brief notify the member that the game in the room started
	* @param reqInfo the buffer request from the client
	* @param data the client's handler and username
	* @return result struct (including buffer)
	*/
	RequestResult handleNotifyStartGame(const RequestInfo& reqInfo, ClientData& data);

	std::unique_ptr<RequestHandlerFactory> _factory;
	std::unique_ptr<NotifyRoomChanges> _triggers;
	std::shared_ptr<Room> _usersRoom; //the room the user in currently
	int _usersRoomID{ 0 };

	static const std::map<ClientCodes, HandleMethod> _actions;
};

