#pragma once
#include "IRequestHandler.h"
#include "EnumCodes.h"
#include "JsonRequestPacketDeserializer.h"
#include "JsonResponsePacketSerializer.h"
#include "NotifyRoomChanges.h"
#include <map>

class RequestHandlerFactory;

/**
* @brief handler for room admins (one who created it)
*/
class RoomAdminRequestHandler : public IRequestHandler
{
public:
	using CheckMathod = void(RoomAdminRequestHandler::*)(const RequestInfo&, ClientData&);
	using HandleMethod = RequestResult(RoomAdminRequestHandler::*)(const RequestInfo&, ClientData&);

	RoomAdminRequestHandler();
	virtual ~RoomAdminRequestHandler() = default;

	/**
	* @brief check if the request is relevant
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
	* @brief close the room the user is managing
	* @param reqInfo the buffer request from the client
	* @param data the client's handler and username
	* @return result struct (including buffer)
	*/
	RequestResult handleCloseRoom(const RequestInfo& reqInfo, ClientData& data);

	/**
	* @brief start the game the user is managing
	* @param reqInfo the buffer request from the client
	* @param data the client's handler and username
	* @return result struct (including buffer)
	*/
	RequestResult handleStartGame(const RequestInfo& reqInfo, ClientData& data);

	/**
	* @brief get the room state (the curr room of the user)
	* @param reqInfo the buffer request from the client
	* @param data the client's handler and username
	* @return result struct (including buffer)
	*/
	RequestResult handleGetRoomState(const RequestInfo& reqInfo, ClientData& data);

	/**
	* @brief notify the admin that one of the members left the room
	* @param reqInfo the buffer request from the client
	* @param data the client's handler and username
	* @return result struct (including buffer)
	*/
	RequestResult handleNotifyMemberLeft(const RequestInfo& reqInfo, ClientData& data);

	
	std::unique_ptr<RequestHandlerFactory> _factory;
	std::unique_ptr<NotifyRoomChanges> _triggers;
	std::shared_ptr<Room> _usersRoom; //the room the user in currently

	static const std::map<ClientCodes, HandleMethod> _actions;
};

