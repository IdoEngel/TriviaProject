#pragma once
#include <algorithm>
#include <map>
#include "IRequestHandler.h"
#include "EnumCodes.h"
#include "JsonRequestPacketDeserializer.h"
#include "JsonResponsePacketSerializer.h"
#include "RequestResponseStructs.h"
#include "ClientConnectionException.h"

class RequestHandlerFactory;

/**
* @brief request handler for menu options (data, statistics, create/join rooms
*/
class MenuRequestHandler : public IRequestHandler
{
public:
	using CheckMethod = void(MenuRequestHandler::*)(const RequestInfo&);
	using HandleMethod = RequestResult(MenuRequestHandler::*)(const RequestInfo&);

	MenuRequestHandler();
	/**
	* @brief checks if the request is relevant and can run it
	* @param reqInfo the buffer request from the client
	* @throw ClientConnectionException if the request is not relevant
	*/
	virtual void isRequestRelevant(const RequestInfo& reqInfo, ClientData& /*NOT IN USE*/) override; //throw ClientConnectionException


	static int* getClicks();

	/**
	* @brief run the request on the server
	* @param reqInfo the buffer request from the client
	* @param cliData the username and handler to update if user logged out 
	* @returns result struct (including buffer)
	*/
	virtual RequestResult handleRequest(const RequestInfo& reqInfo, ClientData& cliData) override;
private:
	/**
	* @brief checks if the logout request is valid and can perform the operation
	* @param reqInfo the buffer request from the client
	* @throw ClientConnectionException if the request is not relevant
	*/
	void isLogOutRelevant(const RequestInfo& reqInfo);
	/**
	* @brief do a logout from the server of the client
	* @param reqInfo the buffer request from the client
	* @returns result struct (including buffer)
	*/
	RequestResult handleLogOut(const RequestInfo& reqInfo);


	/**
	* @brief checks if the request to get players in the room is relevant
	* @param reqInfo the buffer request from the client
	* @throw ClientConnectionException if the request is not relevant
	*/
	void isPlayersInRoomRequestRelevant(const RequestInfo& reqInfo);
	/**
	* @brief do a search in the requested room for the client
	* @param reqInfo the buffer request from the client
	* @returns result struct (including buffer) of all the players in the room
	*/
	RequestResult handlePlayersInRoomRequest(const RequestInfo& reqInfo);

	/**
	* @brief check if the request of joining room is valid and passible
	* @param reqInfo the buffer request from the client
	* @throw ClientConnectionException if the request is not relevant
	*/
	void isJoinRoomRequestRelevant(const RequestInfo& reqInfo);
	/**
	* @brief join the client to the requested room
	* @param  reqInfo the buffer request from the client
	* @return result struct (including buffer)
	*/
	RequestResult handleJoinRoomRequest(const RequestInfo& reqInfo);

	/**
	* @brief checks if the creation of the room is valid
	* @param reqInfo the buffer request from the client
	* @throw ClientConnectionException id the request is not relevant
	*/
	void isCreateRoomRequestRelevant(const RequestInfo& reqInfo);
	/**
	* @brief create a room on the server 
	* @param reqInfo the buffer request from the client
	* @returns result struct (including buffer)
	*/
	RequestResult handleCreateRoomRequest(const RequestInfo& reqInfo);

	/**
	* @brief check if the get request is relevant 
	* @brief the request returns the top 5 players of the game
	* @param reqInfo the buffer request from the client
	* @throw ClientConnectionException if the request is not relevant
	*/
	void isGetHighScoreRequestRelevant(const RequestInfo& reqInfo);
	/**
	* @brief returns the top 5 players
	* @brief the request returns the top 5 players of the game
	* @param reqInfo the buffer request from the client
	* @returns result struct (including buffer)
	*/
	RequestResult handleGetHighScoreRequest(const RequestInfo& reqInfo);

	/**
	* @brief checks if the request of getting the personal statistics is relevant
	* @param reqInfo the buffer request from the client
	* @throw ClientConnectionException if the request is not relevant
	*/
	void isGetPersonalStatsRequestRelevant(const RequestInfo& reqInfo);
	/**
	* @brief gets the personal statistics of the user requested it
	* @param reqInfo the buffer request from the client
	* @returns result struct (including Buffer)
	*/
	RequestResult handleGetPersonalStatsRequest(const RequestInfo& reqInfo);

	/**
	* @brief checks if the request of getting all the rooms is relevant
	* @param reqInfo the buffer request from the client
	* @throw ClientConnectionException if the request is not relevant
	*/
	void isGetRoomsRequestRelevant(const RequestInfo& reqInfo);
	/**
	* @brief gets all the rooms from the server, as client requested
	* @param reqInfo the buffer request from the client
	* @returns result struct (including Buffer)
	*/
	RequestResult handleGetRoomsRequest(const RequestInfo& reqInfo);

	/**
	* @brief check if the notification of closed room relevant
	* @param reqInfo the buffer request from the client
	* @throw ClientConnectionException if the request is not relevant
	*/
	void isNotifyRoomCloseRelevant(const RequestInfo& reqInfo);

	/**
	* @brief handle the notification about room closing
	* @param reqInfo the buffer request from the client
	* @returns result struct (including Buffer)
	*/
	RequestResult handleNotifyRoomCloseRequest(const RequestInfo& reqInfo);

	static const std::map<ClientCodes, CheckMethod> _checks;
	static const std::map<ClientCodes, HandleMethod> _actions;

	std::unique_ptr<RequestHandlerFactory> _factory;

	static int* numberOfClicks;
};

