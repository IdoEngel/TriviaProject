#include "GameRequestHandler.h"
#include "RequestHandlerFactory.h"

const std::vector<ClientCodes> GameRequestHandler::_validCodes = {
	ClientCodes::LEAVE_GAME_REQUEST,
	ClientCodes::GET_QUESTION_REQUEST,
	ClientCodes::SUBMIT_ANS_REQUEST,
	ClientCodes::GET_GAME_RESULTS_REQUEST,
	(ClientCodes)Codes::_NOTIFY_CLOSED_ROOM,
	(ClientCodes)Codes::_NOTIFY_ADMIN_MEMBER_LEFT
};

const std::map<ClientCodes, GameRequestHandler::HandleMethod> GameRequestHandler::_actions = {
	{ClientCodes::LEAVE_GAME_REQUEST, &GameRequestHandler::handleLeaveGameRequest},
	{ClientCodes::GET_QUESTION_REQUEST, &GameRequestHandler::handleGetQuestionRequest},
	{ClientCodes::SUBMIT_ANS_REQUEST, &GameRequestHandler::handleSubmitAnswerRequest},
	{ClientCodes::GET_GAME_RESULTS_REQUEST, &GameRequestHandler::handleGetGameResultRequest},
	{(ClientCodes)Codes::_NOTIFY_CLOSED_ROOM, &GameRequestHandler::handleNotifyCloseGameRequest},
	{(ClientCodes)Codes::_NOTIFY_ADMIN_MEMBER_LEFT, &GameRequestHandler::handleNotifyAdminMemberLeft}
};

GameRequestHandler::GameRequestHandler() : _gameID(0) {
	this->_factory = std::make_unique<RequestHandlerFactory>();
	this->_questionsAnswered = 0;
}

GameRequestHandler::GameRequestHandler(const int gameID) 
: _gameID(gameID) {
	this->_factory = std::make_unique<RequestHandlerFactory>();
	this->_questionsAnswered = 0;

	this->_creationTime = std::chrono::steady_clock::now();
}

void GameRequestHandler::isRequestRelevant(const RequestInfo& reqInfo, ClientData& data) {
	ClientConnectionException::info_t errInfo;
	std::shared_ptr<Room> usersRoom;

	/* general checks - just in case */

	if (data._username.has_value()) {
		usersRoom = NotifyRoomChanges::getUsersRoom(data._username.value(), ALL_MEMBERS);
	}
	else { //if there is no username
		errInfo.clientMsg = "You tried to be in a game without have a username!";
		errInfo.serverPrint = "tried to join a game without a username";
		throw ClientConnectionException(errInfo);
	}

	if (usersRoom.get() == nullptr) { //is null, didnot found a room that the user is in
		errInfo.clientMsg = "To be in a game, you first need to join a room!";
		errInfo.serverPrint = "tried to join a game without being in a room";
		throw ClientConnectionException(errInfo);
	}

	//try to find the users game, if not found (null), throw
	if (this->_factory->getGameManager().findUsersGame(data._username.value()).get() == nullptr) {
		errInfo.clientMsg = "Looks like you are not in a game!";
		errInfo.serverPrint = "tried to prform an action of a room without being in one";
		throw ClientConnectionException(errInfo);
	}

	ClientCodes msgCode = static_cast<ClientCodes>(JsonRequestPacketDeserializer::getMsgCode(reqInfo.buffer));

	//if the std::find method didnot find the right code, code is not void in this state
	if (std::find(GameRequestHandler::_validCodes.begin(), GameRequestHandler::_validCodes.end(), msgCode) ==
		GameRequestHandler::_validCodes.end()) {

		errInfo.clientMsg = "You cannot do an action outside of a game, leave it and try again";
		errInfo.serverPrint = "tried to perform an action that not valid when in a room";
		throw ClientConnectionException(errInfo);
	}
}

RequestResult GameRequestHandler::handleRequest(const RequestInfo& reqInfo, ClientData& data) {
	ClientCodes code = static_cast<ClientCodes>(JsonRequestPacketDeserializer::getMsgCode(reqInfo.buffer));
	GameRequestHandler::HandleMethod method = GameRequestHandler::_actions.at(code);
	
	return (this->*method)(reqInfo, data); //calling the relevant function and return its retrurn value
}


RequestResult GameRequestHandler::handleLeaveGameRequest(const RequestInfo& reqInfo, ClientData& data) {
	LeaveGameResponse res;
	RequestResult answer;

	GameManager& manager = this->_factory->getGameManager();

	//find the game inside the manager...        | remove the player from the room found...
	manager.findUsersGame(data._username.value())->removePlayer(data._username.value());

	res.status = static_cast<unsigned int>(Codes::LEAVE_GAME_SUCCESS);

	answer.buffer = JsonResponsePacketSerializer::serializeLeaveGameResponse(res, TO_STR);
	answer.newHandler = this->_factory->createMenuRequestHandler(); //if leaved game, the room is active and cannot join it


	return answer;
}

RequestResult GameRequestHandler::handleGetQuestionRequest(const RequestInfo& reqInfo, ClientData& data) {
	std::optional<std::shared_ptr<Question>> questionGot;

	GetQuestionResponse res;
	RequestResult answer;

	std::shared_ptr<Game> game = this->_factory->getGameManager().findUsersGame(data._username.value());

	res.status = static_cast<unsigned int>(Codes::GET_QUESTION_SUCCESS);

	questionGot = game->getQuestionForUser(data._username.value());
	
	//if not all of the questions already passed to the user
	if (questionGot.has_value()) {
		res.answers = questionGot.value()->getAnswers();
		res.question = questionGot.value()->getQuestion();
	}
	else { //all the questions passed
		//clear just in case...
		res.answers.clear();
		res.question = "";
	}

	answer.buffer = JsonResponsePacketSerializer::serializeGetQuestionResponse(res, TO_STR);
	answer.newHandler = this->_factory->createGameRequestHandler(game->getGameID());

	return answer;
}

RequestResult GameRequestHandler::handleSubmitAnswerRequest(const RequestInfo& reqInfo, ClientData& data) {
	std::optional<std::shared_ptr<Question>> questionGot;

	SubmitAnswerResponse res;
	SubmitAnswerRequest req;
	RequestResult answer;

	req = JsonRequestPacketDeserializer::deserializeSubmitAnswerRequest(reqInfo.buffer);

	std::shared_ptr<Game> game = this->_factory->getGameManager().findUsersGame(data._username.value());

	questionGot = game->getQuestionForUser(data._username.value());

	res.status = static_cast<unsigned int>(Codes::SUBMIT_ANS_SUCCESS);

	//if not all of the questions already passed to the user
	if (questionGot.has_value()) {
		res.correctAnswerId = questionGot.value()->getCorrectAnsID();

		std::chrono::steady_clock::duration diff = std::chrono::steady_clock::now() - this->_creationTime;//count the of time passed

		game->submitAnswer(data._username.value(), req.answerID,
			std::chrono::duration_cast<std::chrono::seconds>(diff).count()); //count the number of seconds passed
	}
	else { //if there is no questions left INVALID_MSG_ID=5 tells that the user aleady answered the last one
		res.correctAnswerId = INVALID_MSG_ID;
	}

	answer.buffer = JsonResponsePacketSerializer::serializeSubmitAnswerResponse(res, TO_STR);
	answer.newHandler = this->_factory->createGameRequestHandler(game->getGameID());

	return answer;
}

RequestResult GameRequestHandler::handleGetGameResultRequest(const RequestInfo& reqInfo, ClientData& data) {
	std::optional<PlayerResults*> lastPlayerRes = {};
	GetGameResultsResponse* res = nullptr;
	RequestResult answer;

	std::shared_ptr<Game> game = this->_factory->getGameManager().findUsersGame(data._username.value());

	//if the game ended, update DB and send a response
	if (game->checkForEndGame(&SqliteDatabase::get())) {

		res = new GetGameResultsResponse(game->getNumberOfMembers(), true);
		res->updateAll(game->getPlayers());
	}
	else {
		res = new GetGameResultsResponse(game->getNumberOfMembers(), false);
	}
	//all other fields got data from the constructor

	answer.buffer = JsonResponsePacketSerializer::serializeGetGameResultsResponse(*res, TO_STR);
	
	if (res->successful) { // if successful - return to  menu after
		answer.newHandler = this->_factory->createMenuRequestHandler();

		this->_factory->getRoomManager().releaseUser(data._username.value());
		this->_factory->getRoomManager().roomCleanup();
	}
	else { //if not successful - still need to wait fro results, dont change handler
		answer.newHandler = this->_factory->createGameRequestHandler(game->getGameID());
	}
	delete res;

	return answer;
}

RequestResult GameRequestHandler::handleNotifyCloseGameRequest(const RequestInfo& reqInfo, ClientData& data) {
	RequestResult res;

	res.buffer = reqInfo.buffer;
	res.newHandler = this->_factory->createMenuRequestHandler();

	//delete the user from the room
	this->_factory->getRoomManager().releaseUser(data._username.value());
	this->_factory->getRoomManager().roomCleanup();

	return res;
}

RequestResult GameRequestHandler::handleNotifyAdminMemberLeft(const RequestInfo& reqInfo, ClientData& /*NOT IN USE*/) {
	RequestResult res;

	res.buffer = reqInfo.buffer;
	res.newHandler = this->_factory->createGameRequestHandler(this->_gameID);

	return res;
}
