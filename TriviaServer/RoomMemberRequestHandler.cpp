#include "RoomMemberRequestHandler.h"
#include "RequestHandlerFactory.h"

const std::map<ClientCodes, RoomMemberRequestHandler::HandleMethod> RoomMemberRequestHandler::_actions = {
	{ClientCodes::LEAVE_ROOM_REQUEST, &RoomMemberRequestHandler::handleLeaveRoom},
	{ClientCodes::GET_ROOM_STATE_REQUEST, &RoomMemberRequestHandler::handleGetRoomState},
	{(ClientCodes)Codes::_NOTIFY_CLOSED_ROOM, &RoomMemberRequestHandler::handleNotifyCloseRoom},
	{(ClientCodes)Codes::_NOTIFY_GAME_STARTED, &RoomMemberRequestHandler::handleNotifyStartGame}
};

RoomMemberRequestHandler::RoomMemberRequestHandler() {
	this->_factory = std::make_unique<RequestHandlerFactory>();
}

void RoomMemberRequestHandler::isRequestRelevant(const RequestInfo& reqInfo, ClientData& data) {
	//check if the action is valid
	/*checks that the action itself is correct*/
	ClientCodes code = static_cast<ClientCodes>(JsonRequestPacketDeserializer::getMsgCode(reqInfo.buffer));
	if (code != ClientCodes::LEAVE_ROOM_REQUEST && code != ClientCodes::GET_ROOM_STATE_REQUEST &&
		//check for server to server codes (code the server sent to itslef to send to the client later)
		(int)code != (int)Codes::_NOTIFY_CLOSED_ROOM && (int)code != (int)Codes::_NOTIFY_GAME_STARTED) {

		ClientConnectionException::info_t info;
		info.clientMsg = "As a room member, leave the room to perform actions outside of the room";
		info.serverPrint = "tried to do non-room related action when in room";
		throw ClientConnectionException(info);
	}

	//check if there is a room that the curr user is member of
	/*checks that the user is actually in a room*/
	this->_usersRoom = nullptr;

	//check if the value exists (user logged in)
	if (data._username.has_value()) {
		//get the room (or return Empty (nullptr) std::unique_ptr
		this->_usersRoom = NotifyRoomChanges::getUsersRoom(data._username.value(), ALL_MEMBERS);
		this->_usersRoomID = this->_usersRoom->getRoomData().id;
	}

	if (this->_usersRoom) { //if pointer holds value (value returned from NotifyRoomChanges::getUsersRoom())
		this->_triggers = std::make_unique<NotifyRoomChanges>(this->_usersRoom);
	}
	else { //if no value in ptr, throw error (user not an admin in any of the room)
		ClientConnectionException::info_t info;
		info.clientMsg = "You need to be in a room to perform a room action";
		info.serverPrint = "tried to perform a room action on room, when not in a room";
		throw ClientConnectionException(info);
	}
}

RequestResult RoomMemberRequestHandler::handleRequest(const RequestInfo& reqInfo, ClientData& data) {
	ClientCodes code = static_cast<ClientCodes>(JsonRequestPacketDeserializer::getMsgCode(reqInfo.buffer));
	HandleMethod method = RoomMemberRequestHandler::_actions.at(code);

	return (this->*method)(reqInfo, data);
}

RequestResult RoomMemberRequestHandler::handleLeaveRoom(const RequestInfo & reqInfo, ClientData & data) {
	RequestResult res;
	LeaveRoomResponse toSerialize;

	this->_triggers->triggerAdminLeftMember(data._username.value());

	std::shared_ptr<Room> room = this->_factory->getRoomManager().getRoom(this->_usersRoomID);
	room->removeUser(LoggedUser{ data._username.value() });


	toSerialize.status = static_cast<int>(Codes::LEAVE_ROOM_SUCCESS);
	res.buffer = JsonResponsePacketSerializer::serializeLeaveRoomResponse(toSerialize, TO_STR);
	res.newHandler = this->_factory->createMenuRequestHandler();

	return res;
}

RequestResult RoomMemberRequestHandler::handleGetRoomState(const RequestInfo& reqInfo, ClientData& data) {
	RequestResult res;
	GetRoomStateResponse toSerialize;
	RoomData roomMetadata = this->_usersRoom->getRoomData();

	toSerialize.status = static_cast<int>(Codes::GET_ROOM_STATE_SUCCESS);
	toSerialize.players = this->_usersRoom->getAllUsers();

	toSerialize.isActive = roomMetadata.isActive;
	toSerialize.numOfQuestions = roomMetadata.numOfQuestions;
	toSerialize.timePerQuestion = roomMetadata.timePerQuestion;

	res.buffer = JsonResponsePacketSerializer::serializeGetRoomStateResponse(toSerialize, TO_STR);
	res.newHandler = this->_factory->createRoomMemberRequestHandler();

	return res;
}

RequestResult RoomMemberRequestHandler::handleNotifyCloseRoom(const RequestInfo& reqInfo, ClientData& data) {
	RequestResult res;

	/*the buffer is already made by NotifyRoomChanges::triggerCloseRoom, all left is to actually send it to the client
	the buffer got from reqInfo is the one need to send to the client*/

	res.buffer = reqInfo.buffer;
	res.newHandler = this->_factory->createMenuRequestHandler();

	this->_factory->getRoomManager().releaseUser(data._username.value());
	this->_factory->getRoomManager().roomCleanup();
	return res;
}

RequestResult RoomMemberRequestHandler::handleNotifyStartGame(const RequestInfo& reqInfo, ClientData& data) {
	RequestResult res;

	/*the buffer is already made by NotifyRoomChanges::triggerCloseRoom, all left is to actually send it to the client
	the buffer got from reqInfo is the one need to send to the client*/
	std::shared_ptr<Game> game = this->_factory->getGameManager().findUsersGame(data._username.value());

	res.buffer = reqInfo.buffer;
	res.newHandler = this->_factory->createGameRequestHandler(game->getGameID());

	return res;
}
