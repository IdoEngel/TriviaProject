#include "RoomAdminRequestHandler.h"
#include "RequestHandlerFactory.h"


const std::map<ClientCodes, RoomAdminRequestHandler::HandleMethod> RoomAdminRequestHandler::_actions = {
	{ClientCodes::CLOSE_ROOM_REQUEST, &RoomAdminRequestHandler::handleCloseRoom},
	{ClientCodes::START_GAME_REQUEST, &RoomAdminRequestHandler::handleStartGame},
	{ClientCodes::GET_ROOM_STATE_REQUEST, &RoomAdminRequestHandler::handleGetRoomState},
	{(ClientCodes)Codes::_NOTIFY_ADMIN_MEMBER_LEFT, &RoomAdminRequestHandler::handleNotifyMemberLeft}
};

RoomAdminRequestHandler::RoomAdminRequestHandler() {
	this->_factory = std::make_unique<RequestHandlerFactory>();
}


void RoomAdminRequestHandler::isRequestRelevant(const RequestInfo& reqInfo, ClientData& data) {
	//check if the action valid 
	/*checks that the action itself is correct*/
	ClientCodes code = static_cast<ClientCodes>(JsonRequestPacketDeserializer::getMsgCode(reqInfo.buffer));
	if (code != ClientCodes::CLOSE_ROOM_REQUEST && code != ClientCodes::START_GAME_REQUEST &&
		code != ClientCodes::GET_ROOM_STATE_REQUEST && 
		//check for server to server codes (code the server sent to itslef to send to the client later)
		(int)code != (int)Codes::_NOTIFY_ADMIN_MEMBER_LEFT) {
		ClientConnectionException::info_t info;
		info.clientMsg = "As a room admin, close the room to perform actions outside of the room";
		info.serverPrint = "tried to do non-room related action when in room";
		throw ClientConnectionException(info);
	}


	//check if there is a room that the curr user is admin of
	/*checks that the user, with its state in the room, can perform an admin action*/
	this->_usersRoom = nullptr;

	//check if the value exists (user logged in)
	if (data._username.has_value()) {
		//get the room (or return Empty (nullptr) std::unique_ptr
		this->_usersRoom = NotifyRoomChanges::getUsersRoom(data._username.value(), ADMIN_ONLY);
	}

	if (this->_usersRoom) { //if pointer holds value (value returned from NotifyRoomChanges::getUsersRoom())
		this->_triggers = std::make_unique<NotifyRoomChanges>(this->_usersRoom);
	}
	else { //if no value in ptr, throw error (user not an admin in any of the room)
		ClientConnectionException::info_t info;
		info.clientMsg = "You need to be a room admin to perform this action";
		info.serverPrint = "tried to perform an admin action on room, when not an admin";
		throw ClientConnectionException(info);
	}
}

RequestResult RoomAdminRequestHandler::handleRequest(const RequestInfo& reqInfo, ClientData& data) {

	//find the the right method
	ClientCodes code = static_cast<ClientCodes>(JsonRequestPacketDeserializer::getMsgCode(reqInfo.buffer));

	RoomAdminRequestHandler::HandleMethod method = RoomAdminRequestHandler::_actions.at(code);
	return (this->*method)(reqInfo, data);
}


RequestResult RoomAdminRequestHandler::handleCloseRoom(const RequestInfo& reqInfo, ClientData& data) {
	RequestResult res;
	CloseRoomResponse toSerialize;

	this->_triggers->triggerCloseRoom();
	this->_factory->getRoomManager().deleteRoom(this->_usersRoom->getRoomData().id);


	toSerialize.status = static_cast<int>(Codes::CLOSE_ROOM_SUCCESS);
	res.buffer = JsonResponsePacketSerializer::serializeCloseRoomResponse(toSerialize, TO_STR);
	res.newHandler = this->_factory->createMenuRequestHandler();

	return res;
}


RequestResult RoomAdminRequestHandler::handleStartGame(const RequestInfo& reqInfo, ClientData& data) {
	RequestResult res;
	StartGameResponse toSerialize;
	std::shared_ptr<Room> room;
	std::shared_ptr<Game> game;


	//change the state of the game in the RoomManager
	room = this->_factory->getRoomManager().getRoom(this->_usersRoom->getRoomData().id);

	room->startGame();
	game = this->_factory->getGameManager().createGame(room);

	//trigger aftet the creation of the room
	this->_triggers->triggerStartGame();

	toSerialize.status = static_cast<int>(Codes::START_GAME_SUCCESS);
	res.buffer = JsonResponsePacketSerializer::serializeStartGameResponse(toSerialize, TO_STR);
	res.newHandler = this->_factory->createGameRequestHandler(game->getGameID()); //PUT GameRequestHandler later

	return res;
}


RequestResult RoomAdminRequestHandler::handleGetRoomState(const RequestInfo& reqInfo, ClientData& data) {
	RequestResult res;
	GetRoomStateResponse toSerialize;
	RoomData roomMetadata = this->_usersRoom->getRoomData();

	toSerialize.status = static_cast<int>(Codes::GET_ROOM_STATE_SUCCESS);
	toSerialize.players = this->_usersRoom->getAllUsers();

	toSerialize.isActive = roomMetadata.isActive;
	toSerialize.numOfQuestions = roomMetadata.numOfQuestions;
	toSerialize.timePerQuestion = roomMetadata.timePerQuestion;

	res.buffer = JsonResponsePacketSerializer::serializeGetRoomStateResponse(toSerialize, TO_STR);
	res.newHandler = this->_factory->createRoomAdminRequestHandler();

	return res;
}

RequestResult RoomAdminRequestHandler::handleNotifyMemberLeft(const RequestInfo& reqInfo, ClientData& data) {
	RequestResult res;

	/*the buffer is already made by NotifyRoomChanges::triggerAdminLeftMember, all left is to actually send it to the client
	the buffer got from reqInfo is the one need to send to the client*/

	res.buffer = reqInfo.buffer;
	res.newHandler = this->_factory->createRoomAdminRequestHandler();

	return res;
}
