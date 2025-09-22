#include "RequestHandlerFactory.h"

RequestHandlerFactory::RequestHandlerFactory() 
	:_db(SqliteDatabase::get()), _loginManager(LoginManager::get()), _roomManager(RoomManager::get()), 
	_statisticsManager(StatisticsManager::get()), _gameManager(GameManager::get()) { }

std::shared_ptr<LoginRequestHandler> RequestHandlerFactory::createLoginRequestHandler() {
	return std::make_shared<LoginRequestHandler>();
}

LoginManager& RequestHandlerFactory::getLoginManager() {
	return this->_loginManager;
}

std::shared_ptr<MenuRequestHandler> RequestHandlerFactory::createMenuRequestHandler() {
	return std::make_shared<MenuRequestHandler>();
}

std::shared_ptr<RoomAdminRequestHandler> RequestHandlerFactory::createRoomAdminRequestHandler() {
	return std::make_shared<RoomAdminRequestHandler>();
}

std::shared_ptr<RoomMemberRequestHandler> RequestHandlerFactory::createRoomMemberRequestHandler() {
	return std::make_shared<RoomMemberRequestHandler>();
}

std::shared_ptr<GameRequestHandler> RequestHandlerFactory::createGameRequestHandler(const int gameID) {
	return std::make_shared<GameRequestHandler>(gameID);
}

RoomManager& RequestHandlerFactory::getRoomManager() {
	return this->_roomManager;
}

StatisticsManager& RequestHandlerFactory::getStatisticsManager() {
	return this->_statisticsManager;
}

GameManager& RequestHandlerFactory::getGameManager() {
	return this->_gameManager;
}
