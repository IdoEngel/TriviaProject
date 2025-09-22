#include "MenuRequestHandler.h"
#include "RequestHandlerFactory.h"

int* MenuRequestHandler::numberOfClicks = new int[9];

MenuRequestHandler::MenuRequestHandler() { 
	this->_factory = std::make_unique<RequestHandlerFactory>(); 
}

const std::map<ClientCodes, MenuRequestHandler::CheckMethod> MenuRequestHandler::_checks = {
	{ClientCodes::LOG_OUT_REQUEST, &MenuRequestHandler::isLogOutRelevant},
	{ClientCodes::PLAYERS_IN_ROOM_REQUEST, &MenuRequestHandler::isPlayersInRoomRequestRelevant},
	{ClientCodes::JOIN_ROOM_REQUEST, &MenuRequestHandler::isJoinRoomRequestRelevant},
	{ClientCodes::CREATE_ROOM_REQUEST, &MenuRequestHandler::isCreateRoomRequestRelevant},
	{ClientCodes::GET_HIGH_SCORE_REQUEST, &MenuRequestHandler::isGetHighScoreRequestRelevant},
	{ClientCodes::GET_PRSONAL_STAT_REQUEST, &MenuRequestHandler::isGetPersonalStatsRequestRelevant},
	{ClientCodes::GET_ROOMS_REQUEST, &MenuRequestHandler::isGetRoomsRequestRelevant},
	{(ClientCodes)Codes::_NOTIFY_CLOSED_ROOM, &MenuRequestHandler::isNotifyRoomCloseRelevant}
};

const std::map<ClientCodes, MenuRequestHandler::HandleMethod> MenuRequestHandler::_actions = {
	{ClientCodes::LOG_OUT_REQUEST,  &MenuRequestHandler::handleLogOut},
	{ClientCodes::PLAYERS_IN_ROOM_REQUEST, &MenuRequestHandler::handlePlayersInRoomRequest},
	{ClientCodes::JOIN_ROOM_REQUEST, &MenuRequestHandler::handleJoinRoomRequest},
	{ClientCodes::CREATE_ROOM_REQUEST, &MenuRequestHandler::handleCreateRoomRequest},
	{ClientCodes::GET_HIGH_SCORE_REQUEST, &MenuRequestHandler::handleGetHighScoreRequest},
	{ClientCodes::GET_PRSONAL_STAT_REQUEST, &MenuRequestHandler::handleGetPersonalStatsRequest},
	{ClientCodes::GET_ROOMS_REQUEST, &MenuRequestHandler::handleGetRoomsRequest},
	{(ClientCodes)Codes::_NOTIFY_CLOSED_ROOM, &MenuRequestHandler::handleNotifyRoomCloseRequest}
};

int* MenuRequestHandler::getClicks() {
	return MenuRequestHandler::numberOfClicks;
}

/*GENERAL funcs*/

void MenuRequestHandler::isRequestRelevant(const RequestInfo& reqInfo, ClientData& /*NOT IN USE*/) {
	//find the the right method
	ClientCodes code = static_cast<ClientCodes>(JsonRequestPacketDeserializer::getMsgCode(reqInfo.buffer));

	try {
		MenuRequestHandler::CheckMethod method = MenuRequestHandler::_checks.at(code);
		(this->*method)(reqInfo); //calling the relevant function
	}
	catch (const std::out_of_range& e) { //catch if the .at() faild, means that element with that key didnot found
		ClientConnectionException::info_t info;
		info.clientMsg = "The action is not valid in your state (maybe you need to enter/create a room first?)";
		info.serverPrint = "tried to perform an action that invalid to the curr state in the server";
		throw ClientConnectionException(info); //throw that catches later
	}
}

RequestResult MenuRequestHandler::handleRequest(const RequestInfo& reqInfo, ClientData& cliData) {
	//find the the right method
	ClientCodes code = static_cast<ClientCodes>(JsonRequestPacketDeserializer::getMsgCode(reqInfo.buffer));

	MenuRequestHandler::HandleMethod method = MenuRequestHandler::_actions.at(code);

	if ((int)code != (int)Codes::_NOTIFY_CLOSED_ROOM) {
		this->numberOfClicks[(int)code - (int)ClientCodes::LOG_OUT_REQUEST]++;
	}

	//if user logged out, need to change the username to an empty state
	if (code == ClientCodes::LOG_OUT_REQUEST) {
		cliData._username = {};
	}
	return (this->*method)(reqInfo); //calling the relevant function and return its retrurn value
}

/*LOGOUT funcs*/

void MenuRequestHandler::isLogOutRelevant(const RequestInfo& reqInfo) {
	LogoutRequest logoutData = JsonRequestPacketDeserializer::deserializeLogoutRequest(reqInfo.buffer);

	if (!this->_factory->getLoginManager().isLoggedIn(logoutData.username)) {
		ClientConnectionException::info_t errInfo;
		errInfo.clientMsg = "You are not logged in the server";
		errInfo.serverPrint = "The user " + logoutData.username + " is not logged in";
		throw ClientConnectionException(errInfo);
	}
}

RequestResult MenuRequestHandler::handleLogOut(const RequestInfo& reqInfo) {
	RequestResult res;
	LogoutResponse logoutOutput;

	LogoutRequest logoutData = JsonRequestPacketDeserializer::deserializeLogoutRequest(reqInfo.buffer);

	//logout the user
	this->_factory->getLoginManager().logout(logoutData.username);

	logoutOutput.status = static_cast<unsigned int>(Codes::LOG_OUT_SUCCESS); 

	//fill the response
	res.newHandler = nullptr;
	res.buffer = JsonResponsePacketSerializer::serializeLogoutResponse(logoutOutput, TO_STR);

	return res;
}

/*PLAYERS IN ROOM funcs*/

void MenuRequestHandler::isPlayersInRoomRequestRelevant(const RequestInfo& reqInfo) {
	GetPlayersInRoomRequest res;

	res = JsonRequestPacketDeserializer::deserializeGetPlayersRequest(reqInfo.buffer);

	if (!this->_factory->getRoomManager().isRoomExists(res.roomID)) {
		ClientConnectionException::info_t errInfo;
		errInfo.clientMsg = "the room does not exists!";
		errInfo.serverPrint = "the room user tried to get its info not exists";
		throw ClientConnectionException(errInfo);
	}
}

RequestResult MenuRequestHandler::handlePlayersInRoomRequest(const RequestInfo& reqInfo) {
	RequestResult res;
	GetPlayersInRoomResponse getInput;
	std::shared_ptr<Room> roomRequested;

	//get room ID to get its players
	GetPlayersInRoomRequest roomID = JsonRequestPacketDeserializer::deserializeGetPlayersRequest(reqInfo.buffer);

	roomRequested = this->_factory->getRoomManager().getRoom(roomID.roomID);

	getInput.players.reserve(roomRequested->getAllUsers().size()); //the output container in the needed size

	//get the data to the serializer format
	std::copy(roomRequested->getAllUsers().cbegin(), roomRequested->getAllUsers().cend(), getInput.players.begin());
	getInput.status = static_cast<unsigned int>(Codes::GET_PLAYERS_IN_ROOM_SUCCESS);
	
	res.buffer = JsonResponsePacketSerializer::serializeGetPlayersInRoomResponse(getInput, TO_STR);
	res.newHandler = this->_factory->createMenuRequestHandler();

	return res;
}

/*JOIN ROOM funcs*/

void MenuRequestHandler::isJoinRoomRequestRelevant(const RequestInfo& reqInfo) {
	ClientConnectionException::info_t errInfo;
	bool isRoomExists = false;
	bool isUserAlreadyInRoom = false;
	bool isUserInOtherRoom = false;
	bool isRoomFull = false;
	bool isRoomActive = false;
	std::vector<std::shared_ptr<Room>> allRooms; //all the rooms in the server

	//check if room exists
	JoinRoomRequest reqFields = JsonRequestPacketDeserializer::deserializeJoinRoomRequest(reqInfo.buffer);
	isRoomExists = this->_factory->getRoomManager().isRoomExists(reqFields.roomID);

	//if the room exists
	if (isRoomExists) {
		//check if user in this room
		isUserAlreadyInRoom = this->_factory->getRoomManager().getRoom(reqFields.roomID)->doesUserInRoom(reqFields.username);

		//check if the user on another room
		allRooms = this->_factory->getRoomManager().getRooms();
		for (const std::shared_ptr<Room>& rm : allRooms) {
			//isUserInOtherRoom starts as false
			isUserInOtherRoom = isUserInOtherRoom ||
				//stays true if one time it was true (user was in one of the rooms
				rm->doesUserInRoom(reqFields.username);
		}

		//checks if the room is full
		isRoomFull = this->_factory->getRoomManager().getRoom(reqFields.roomID)->isRoomFull();

		isRoomActive = this->_factory->getRoomManager().getRoom(reqFields.roomID)->isActive();
	}

	/*Throw exceptions*/
	if (!isRoomExists) { //room must be exists to join it
		errInfo.clientMsg = "The room not exists on the server";
		errInfo.serverPrint = "tried to access non-existing room";
		throw ClientConnectionException(errInfo);
	}
	if (isRoomFull) { //if the room full (if full, cannot join it)
		errInfo.clientMsg = "The room is in its full capacity";
		errInfo.serverPrint = "tried to join a full room";
		throw ClientConnectionException(errInfo);
	}
	if (isRoomActive) {
		errInfo.clientMsg = "The room is already active!";
		errInfo.serverPrint = "tried to join a room with an active game";
		throw ClientConnectionException(errInfo);
	}
	if (isUserAlreadyInRoom) { //cannot join room twice
		errInfo.clientMsg = "You are already in this room";
		errInfo.serverPrint = "Client already in this room";
		throw ClientConnectionException(errInfo);
	}
	if (isUserInOtherRoom) {
		errInfo.clientMsg = "You cannot join two rooms in parallel!";
		errInfo.serverPrint = "tried to join a room with out leave previus";
		throw ClientConnectionException(errInfo);
	}
}


RequestResult MenuRequestHandler::handleJoinRoomRequest(const RequestInfo& reqInfo) {
	RequestResult res;
	JoinRoomResponse reqInput;

	JoinRoomRequest reqFields = JsonRequestPacketDeserializer::deserializeJoinRoomRequest(reqInfo.buffer);

	this->_factory->getRoomManager().getRoom(reqFields.roomID)->addUser(reqFields.username);

	reqInput.status = static_cast<unsigned int>(Codes::JOIN_ROOM_SUCCESS);
	res.buffer = JsonResponsePacketSerializer::serializeJoinRoomResponse(reqInput, TO_STR);

	res.newHandler = this->_factory->createRoomMemberRequestHandler();

	return res;
}

/*CREATE ROOM funcs*/

void MenuRequestHandler::isCreateRoomRequestRelevant(const RequestInfo& reqInfo) {
	ClientConnectionException::info_t errInfo;
	CreateRoomRequest reqFields;
	bool isUserLoggedIn = false;
	bool isRoomNameExists = false;
	std::vector<std::shared_ptr<Room>> allRooms;

	reqFields = JsonRequestPacketDeserializer::deserializeCreateRoomRequest(reqInfo.buffer);

	isUserLoggedIn = this->_factory->getLoginManager().isLoggedIn(reqFields.playerName.username);

	/*cannot create a room with the same name*/
	allRooms = this->_factory->getRoomManager().getRooms();
	for (const std::shared_ptr<Room>& rm : allRooms) {
		//if the room name already in the system
		if (rm->getRoomData().name == reqFields.roomName) {
			isRoomNameExists = true;
		}
	}

	if (!isUserLoggedIn) { //if the user is not logged in
		errInfo.clientMsg = "You MUST be logged in the create a room!";
		errInfo.serverPrint = "tried to create a room while not logged in";
		throw ClientConnectionException(errInfo);
	}
	if (isRoomNameExists) {
		errInfo.clientMsg = "A room with the same name is already exists";
		errInfo.serverPrint = "tried to create a room with a name of another room";
		throw ClientConnectionException(errInfo);
	}
}

RequestResult MenuRequestHandler::handleCreateRoomRequest(const RequestInfo& reqInfo) {
	RequestResult res;
	CreateRoomRequest reqFields;
	RoomData metadata;

	CreateRoomResponse reqInput; //to serialize

	reqFields = JsonRequestPacketDeserializer::deserializeCreateRoomRequest(reqInfo.buffer);

	//fill in the metadata data to create a room
	metadata.id = this->_factory->getRoomManager().getLargestID()+1; //incroment the current biggest ID to get the next one
	metadata.isActive = false;
	metadata.maxPlayers = reqFields.maxPlayers;
	metadata.name = reqFields.roomName;
	metadata.numOfQuestions = reqFields.numOfQuestions;
	metadata.timePerQuestion = reqFields.timePerQuestion;

	this->_factory->getRoomManager().createRoom(reqFields.playerName,/*admin*/reqFields.playerName.username, metadata);

	//struct to serialize
	reqInput.roomID = metadata.id;
	reqInput.status = static_cast<unsigned int>(Codes::CREATE_ROOM_SUCCESS);

	//struct to return
	res.buffer = JsonResponsePacketSerializer::serializeCreateRoomResponse(reqInput, TO_STR);
	res.newHandler = this->_factory->createRoomAdminRequestHandler();

	return res;
}

/*GET TOP 5 funcs*/

void MenuRequestHandler::isGetHighScoreRequestRelevant(const RequestInfo& reqInfo) {
	/*No checks are available, only the request code is sent
	always true. don't throw exception*/
}

RequestResult MenuRequestHandler::handleGetHighScoreRequest(const RequestInfo& reqInfo) {
	RequestResult res;
	std::vector<UserStatistics> topFive;

	GetHighScoreResponse reqInput; //to serialize

	reqInput.statistics = this->_factory->getStatisticsManager().getHighScore();
	reqInput.status = static_cast<unsigned int>(Codes::GET_HIGN_SCORE_TOP_FIVE_SUCCESS);

	res.buffer = JsonResponsePacketSerializer::serializeGetHighScoreResponse(reqInput, TO_STR);
	res.newHandler = this->_factory->createMenuRequestHandler();

	return res;
}

/*PERSONAL STATS funcs*/

void MenuRequestHandler::isGetPersonalStatsRequestRelevant(const RequestInfo& reqInfo) {
	ClientConnectionException::info_t errInfo;
	GetPersonalStatsRequest reqFields;

	reqFields = JsonRequestPacketDeserializer::deserializeGetPersonalStatsRequest(reqInfo.buffer);

	//if the user is not logged in, cannot get the info
	if (!this->_factory->getLoginManager().isLoggedIn(reqFields.username.username)) {
		errInfo.clientMsg = "You need to login to get the relevant statistics for you!";
		errInfo.serverPrint = "tried to get personal stats without logging in";
		throw ClientConnectionException(errInfo);
	}
}

RequestResult MenuRequestHandler::handleGetPersonalStatsRequest(const RequestInfo& reqInfo) {
	RequestResult res;
	GetPersonalStatsRequest reqFields;
	GetPersonalStatsResponse reqInput;

	reqFields = JsonRequestPacketDeserializer::deserializeGetPersonalStatsRequest(reqInfo.buffer);

	//fill in the data to serialize
	reqInput.statistics = this->_factory->getStatisticsManager().getUserStatistics(reqFields.username.username);
	reqInput.status = static_cast<unsigned int>(Codes::GET_PERSONAL_STATS_SUCCESS);

	res.buffer = JsonResponsePacketSerializer::serializeGetPersonalStatsResponse(reqInput, TO_STR);
	res.newHandler = this->_factory->createMenuRequestHandler();

	return res;
}

/*GET ROOMS funcs*/

void MenuRequestHandler::isGetRoomsRequestRelevant(const RequestInfo& reqInfo) {
	/*No checks are available, only the request code is sent
	always true. don't throw exception*/
}

RequestResult MenuRequestHandler::handleGetRoomsRequest(const RequestInfo& reqInfo) {
	RequestResult res;
	GetRoomsResponse reqInput;
	std::vector<std::shared_ptr<Room>> allRooms;

	
	reqInput.status = static_cast<unsigned int>(Codes::GET_ROOMS_SUCCESS);

	this->_factory->getRoomManager().roomCleanup(); //be sure there is no empty room
	allRooms = this->_factory->getRoomManager().getRooms();

	//get into 'reqInput.rooms' vector the only the RoomData struct of each room
	reqInput.rooms.resize(allRooms.size());
	std::transform(allRooms.begin(), allRooms.end(), reqInput.rooms.begin(),
		[](const std::shared_ptr<Room>& rm) -> RoomData {
			return rm->getRoomData();
		});


	res.buffer = JsonResponsePacketSerializer::serializeGetRoomsResponse(reqInput, TO_STR);
	res.newHandler = this->_factory->createMenuRequestHandler();

	return res;
}

void MenuRequestHandler::isNotifyRoomCloseRelevant(const RequestInfo& reqInfo) {
	/**
	* a shadow-function - don't need to check anything, 
	* here in case of crash of one of the users and notify changas
	*/
}

RequestResult MenuRequestHandler::handleNotifyRoomCloseRequest(const RequestInfo& reqInfo) {
	RequestResult res;

	res.buffer = reqInfo.buffer;
	res.newHandler = this->_factory->createMenuRequestHandler();

	return res;
}
