#pragma once
#include <memory>
#include "SqliteDatabase.h"
#include "LoginRequestHandler.h"
#include "LoginManager.h"
#include "MenuRequestHandler.h"
#include "RoomManager.h"
#include "StatisticsManager.h"
#include "GameRequestHandler.h"
#include "GameManager.h"
#include "RoomAdminRequestHandler.h"
#include "RoomMemberRequestHandler.h"


class RoomAdminRequestHandler;
class RoomMemberRequestHandler;
class LoginRequestHandler;
class MenuRequestHandler;
class GameRequestHandler;

class RequestHandlerFactory
{
public:
	RequestHandlerFactory();
	~RequestHandlerFactory() = default;

	/**
	* @brief get an instance of LoginRequestHandler
	* @returns shared_ptr of LoginRequestHandler
	*/
	std::shared_ptr<LoginRequestHandler> createLoginRequestHandler();

	/**
	* @brief creates an instance of MenuRequestHandler
	* @returns shared_ptr of MenuRequestHandler
	*/
	std::shared_ptr<MenuRequestHandler> createMenuRequestHandler();

	/**
	* @brief creates an instance of RoomAdminRequestHandler
	* @returns shared_ptr of RoomAdminRequestHandler
	*/
	std::shared_ptr<RoomAdminRequestHandler> createRoomAdminRequestHandler();

	/**
	* @brief creates an instance of RoomMemberRequestHandler
	* @returns shared_ptr of RoomMemberRequestHandler
	*/
	std::shared_ptr<RoomMemberRequestHandler> createRoomMemberRequestHandler();

	/**
	* @brief creates an instance of GameRequestHandler
	* @param gameID the id of the game currently the user is in
	* @returns shared_ptr of GameRequestHandler
	*/
	std::shared_ptr<GameRequestHandler> createGameRequestHandler(const int gameID);

	/**
	* @brief returns the class that allow communication with the DB
	* @returns LoginManager class that allow signup and log in
	*/
	LoginManager& getLoginManager();

	/**
	* @brief returns the class that manages the game rooms in the server
	* @returns RoomManager class that allow create, join, delete and get  info of rooms
	*/
	RoomManager& getRoomManager();

	/**
	* @brief returns the class that manages and calcs the statistics
	* @returns StatisticsManager class that calcs the statistics
	*/
	StatisticsManager& getStatisticsManager();

	/**
	* @brief returns the class that managers the games 
	* @returns GameManager class
	*/
	GameManager& getGameManager();

private:
	IDatabase& _db;
	LoginManager& _loginManager;
	RoomManager& _roomManager;
	StatisticsManager& _statisticsManager;
	GameManager& _gameManager;
};

