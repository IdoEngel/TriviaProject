#include "JsonResponsePacketSerializer.h"


Buffer JsonResponsePacketSerializer::serializeLoginResponse(const LoginResponse& loginData, const bool toStr) {
	Buffer bfRet;
	nlohmann::json js;

	//pack into json to serialize
	js[serverFields::general::CODE] = loginData.status;

	JsonResponsePacketSerializer::createBuffer(&bfRet, js, (uint8_t)loginData.status, toStr);

	return bfRet;
}

Buffer JsonResponsePacketSerializer::serializeSignUpResponse(const SignupResponse& signupData, const bool toStr) {
	Buffer bfRet;

	nlohmann::json js;

	//pack into json to serialize
	js[serverFields::general::CODE] = signupData.status;

	JsonResponsePacketSerializer::createBuffer(&bfRet, js, (uint8_t)signupData.status, toStr);

	return bfRet;
}

Buffer JsonResponsePacketSerializer::serializeErrorResponse(const ErrorResponse & errData, const bool toStr) {
	Buffer bfRet;

	//pack into json to serialize
	nlohmann::json js;
	js[serverFields::general::CODE] = (uint8_t)Codes::GENERAL_ERROR;
	js[serverFields::general::ERR_MSG] = errData.message;

	JsonResponsePacketSerializer::createBuffer(&bfRet, js, (uint8_t)Codes::GENERAL_ERROR, toStr);

	return bfRet;
}

Buffer JsonResponsePacketSerializer::serializeLogoutResponse(const LogoutResponse& logoutData, const bool toStr) {
	Buffer bfRet;

	//pack into json to serialize
	nlohmann::json js;
	js[serverFields::general::CODE] = logoutData.status;

	JsonResponsePacketSerializer::createBuffer(&bfRet, js, logoutData.status, toStr);
	return bfRet;
}

Buffer JsonResponsePacketSerializer::serializeGetRoomsResponse(const GetRoomsResponse& getRoomsData, const bool toStr) {
	/*
	* format: 
	* {
	*	"code" : getRoomsData.status,
	*	"rooms" : {
	*			<rooms from JsonResponsePacketSerializer::serializeRoomDataStruct>
	*		}
	* }
	*/
	Buffer bfRet;

	//pack into json to serialize
	nlohmann::json js;
	js[serverFields::general::CODE] = getRoomsData.status;
	//create array of rooms
	js[serverFields::rooms::_ROOMS_ARR] = nlohmann::json::object();

	//for every room, insert the data into the json
	for (const RoomData& data : getRoomsData.rooms) {
		JsonResponsePacketSerializer::serializeRoomDataStruct(&(js[serverFields::rooms::_ROOMS_ARR]), data);
	}

	JsonResponsePacketSerializer::createBuffer(&bfRet, js, (uint8_t)getRoomsData.status, toStr);
	return bfRet;
}

Buffer JsonResponsePacketSerializer::serializeGetPlayersInRoomResponse(const GetPlayersInRoomResponse& getPlayersInRoomData, const bool toStr) {
	/*
	* format:
	* {
	*	"code" : getPlayersInRoomData.status,
	*	"players" : [
	*			<all the players names - std::string>
	*		]
	* }
	*/
	Buffer bfRet;

	//pack into json to serialize
	nlohmann::json js;
	js[serverFields::general::CODE] = getPlayersInRoomData.status;

	//create an array of loggedUsers
	js[serverFields::general::_PLYAERS_ARR] = nlohmann::json::array();
	for (const LoggedUser& usr : getPlayersInRoomData.players) { //push into the array
		js[serverFields::general::_PLYAERS_ARR].push_back(usr.username);
	}

	JsonResponsePacketSerializer::createBuffer(&bfRet, js, (uint8_t)getPlayersInRoomData.status, toStr);
	return bfRet;
}

Buffer JsonResponsePacketSerializer::serializeGetHighScoreResponse(const GetHighScoreResponse& getHighScoreData, const bool toStr) {
	/*
	* format:
	* {
	*	"code" : getHighScoreData.status,	
	*	"players" : {
	*			"<playerName>" : {<statistics>},
	*			...
	*		}
	* }
	*/
	Buffer bfRet;

	//pack into json to serialize
	nlohmann::json js;
	js[serverFields::general::CODE] = getHighScoreData.status;

	js[serverFields::general::_PLYAERS_ARR] = nlohmann::json::object();
	for (const UserStatistics& us : getHighScoreData.statistics) {
		JsonResponsePacketSerializer::serializeUserStatisticsStruct(&(js[serverFields::general::_PLYAERS_ARR]), us);
	}

	JsonResponsePacketSerializer::createBuffer(&bfRet, js, (uint8_t)getHighScoreData.status, toStr);
	return bfRet;
}

Buffer JsonResponsePacketSerializer::serializeGetPersonalStatsResponse(const GetPersonalStatsResponse& getPersonalStatsData, const bool toStr) {
	Buffer bfRet;

	nlohmann::json js;

	//insert to json
	js[serverFields::general::CODE] = getPersonalStatsData.status;
	JsonResponsePacketSerializer::serializeUserStatisticsStruct(&js, getPersonalStatsData.statistics);

	JsonResponsePacketSerializer::createBuffer(&bfRet, js, (uint8_t)getPersonalStatsData.status, toStr);

	return bfRet;
}

Buffer JsonResponsePacketSerializer::serializeJoinRoomResponse(const JoinRoomResponse& joinRoomData, const bool toStr) {
	Buffer bfRet;

	nlohmann::json js;

	//insert to json
	js[serverFields::general::CODE] = joinRoomData.status;
	JsonResponsePacketSerializer::createBuffer(&bfRet, js, (uint8_t)joinRoomData.status, toStr);

	return bfRet;
}

Buffer JsonResponsePacketSerializer::serializeCreateRoomResponse(const CreateRoomResponse& createRoomData, const bool toStr) {
	Buffer bfRet;

	nlohmann::json js;

	//insert to json
	js[serverFields::general::CODE] = createRoomData.status;
	js[serverFields::rooms::ID] = createRoomData.roomID;
	JsonResponsePacketSerializer::createBuffer(&bfRet, js, (uint8_t)createRoomData.status, toStr);

	return bfRet;
}

Buffer JsonResponsePacketSerializer::serializeCloseRoomResponse(const CloseRoomResponse& closeRoomData, const bool toStr) {
	Buffer bfRet;

	nlohmann::json js;

	//insert to json
	js[serverFields::general::CODE] = closeRoomData.status;
	JsonResponsePacketSerializer::createBuffer(&bfRet, js, (uint8_t)closeRoomData.status, toStr);

	return bfRet;
}

Buffer JsonResponsePacketSerializer::serializeStartGameResponse(const StartGameResponse& startGameData, const bool toStr) {
	Buffer bfRet;

	nlohmann::json js;

	//insert to json
	js[serverFields::general::CODE] = startGameData.status;
	JsonResponsePacketSerializer::createBuffer(&bfRet, js, (uint8_t)startGameData.status, toStr);

	return bfRet;
}

Buffer JsonResponsePacketSerializer::serializeGetRoomStateResponse(const GetRoomStateResponse& getRoomStateData, const bool toStr) {
	/*
	Full Json Format Example:
		{"code":24,
		"data":{
			"isActive":true,
			"questionCount":15,
			"questionTimer":45},
		"players":["One","Two","Three"]}
	*/
	Buffer bfRet;

	nlohmann::json js;

	//insert to json
	js[serverFields::general::CODE] = getRoomStateData.status;
	JsonResponsePacketSerializer::serializeGetRoomStateStruct(&js, getRoomStateData);

	std::cout << js << std::endl;
	JsonResponsePacketSerializer::createBuffer(&bfRet, js, (uint8_t)getRoomStateData.status, toStr);

	return bfRet;
}

Buffer JsonResponsePacketSerializer::serializeLeaveRoomResponse(const LeaveRoomResponse& leaveRoomData, const bool toStr) {
	Buffer bfRet;

	nlohmann::json js;

	//insert to json
	js[serverFields::general::CODE] = leaveRoomData.status;

	JsonResponsePacketSerializer::createBuffer(&bfRet, js, (uint8_t)leaveRoomData.status, toStr);

	return bfRet;
}

Buffer JsonResponsePacketSerializer::serializeNotifyAdminAboutLeftMemberResponse(const NotifyAdminAboutLeftMember& data, const bool toStr) {
	Buffer bfRet;

	nlohmann::json js;

	//insert to json
	js[serverFields::general::CODE] = data.status;
	js[serverFields::rooms::LEFT_PLAYER] = data.memberUsername;

	JsonResponsePacketSerializer::createBuffer(&bfRet, js, (uint8_t)data.status, toStr);

	return bfRet;
}

Buffer JsonResponsePacketSerializer::serializeLeaveGameResponse(const LeaveGameResponse& data, const bool toStr) {
	Buffer bfRet;

	nlohmann::json js;

	//insert to json
	js[serverFields::general::CODE] = data.status;

	JsonResponsePacketSerializer::createBuffer(&bfRet, js, (uint8_t)data.status, toStr);

	return bfRet;
}

Buffer JsonResponsePacketSerializer::serializeGetQuestionResponse(const GetQuestionResponse& data, const bool toStr) {
	Buffer bfRet;

	nlohmann::json js;

	//insert to json
	js[serverFields::general::CODE] = data.status;
	js[serverFields::game::QUESTION] = data.question;

	js[serverFields::game::_ANSWERS_ARR] = nlohmann::json::object();
	JsonResponsePacketSerializer::serializeMapOfAnswersToAQuestion(&(js[serverFields::game::_ANSWERS_ARR]), data);

	
	JsonResponsePacketSerializer::createBuffer(&bfRet, js, (uint8_t)data.status, toStr);

	return bfRet;
}

Buffer JsonResponsePacketSerializer::serializeSubmitAnswerResponse(const SubmitAnswerResponse& data, const bool toStr) {
	Buffer bfRet;

	nlohmann::json js;

	//insert to json
	js[serverFields::general::CODE] = data.status;
	js[serverFields::game::CORRECT_ANS_ID] = data.correctAnswerId;

	JsonResponsePacketSerializer::createBuffer(&bfRet, js, (uint8_t)data.status, toStr);

	return bfRet;
}

Buffer JsonResponsePacketSerializer::serializeGetGameResultsResponse(const GetGameResultsResponse& data, const bool toStr) {
	Buffer bfRet;

	nlohmann::json js;

	//insert to json
	js[serverFields::general::CODE] = data.status;
	js[serverFields::game::IS_SUCCESSFUL] = data.successful;

	js[serverFields::general::_PLYAERS_ARR] = nlohmann::json::object();
	JsonResponsePacketSerializer::serializeVectorOfPlayerResult(&(js[serverFields::general::_PLYAERS_ARR]), data.results);

	JsonResponsePacketSerializer::createBuffer(&bfRet, js, (uint8_t)data.status, toStr);

	return bfRet;
}

void JsonResponsePacketSerializer::createBuffer(Buffer* bf, const nlohmann::json& js, const uint8_t status, const bool toStr) {
	std::vector<uint8_t> raw;
	std::string rawStr = "";

	unsigned int jsonSize = 0;
	unsigned char jsonSizeBites[JSON_LEN_BYTES_FROM_SERVER] = { 0 };

	if (toStr) {
		rawStr = js.dump();
		raw.insert(raw.begin(), rawStr.cbegin(), rawStr.cend());
	}
	else {
		raw = nlohmann::json::to_bson(js);
	}

	//push code and json len
	bf->data.push_back(status);

	jsonSize = raw.size() * sizeof(uint8_t);
	JsonResponsePacketSerializer::transform(jsonSizeBites, jsonSize);

	int i = 0;
	for (i = 0; i < JSON_LEN_BYTES_FROM_SERVER; i++) {
		bf->data.push_back(jsonSizeBites[i]);
	}

	bf->data.insert(bf->data.end(), raw.cbegin(), raw.cend()); //copy to the end of bfRet

	raw.clear();
}

Buffer JsonResponsePacketSerializer::toBytesBuffer(char* src, const int len) {
	Buffer ret;

	int i = 0;
	for (i = 0; i < len; i++) {
		ret.data.push_back(static_cast<uint8_t>(src[i]));
	}

	return ret;
}

void JsonResponsePacketSerializer::serializeMapOfAnswersToAQuestion(nlohmann::json* js, const GetQuestionResponse& data) {
	/*
	GetQuestionResponse fields:
		unsigned int status;
		std::string question;
		std::map<unsigned int, std::string> answers;
	format: <the question>, { <ans number> : <ans>, ... }
		example: {"1" : "Paris", "2" : "Tel Aviv", ...}
	*/

	for (auto [ans_num, ans] : data.answers) {
		(*js)[std::to_string(ans_num)] = ans; //keys MUST be string typed
	}
}

void JsonResponsePacketSerializer::serializeVectorOfPlayerResult(nlohmann::json* js, const std::vector<PlayerResults>& data) {
	/*
	PlayerResults fields:
		std::string username;
		unsigned int correctAnsCount;
		unsigned int wrongAnsCount;
		float aveAnsTime;
	format: { <username> : { "correctAnsCount" : <correctAnsCount>, "wrongAnsCount" : <wrongAnsCount>, "aveAnsTime" : <aveAnsTime> }, ... }
		example: { "Amit" : { "correctAnsCount": 5, "wrongAnsCount" : 7, "aveAnsTime" : 17.5 }, ... }
	*/

	for (const PlayerResults& res : data) {
		(*js)[res.username] = {
			{serverFields::game::CORRECT_ANS_COUNT, res.correctAnsCount}, //a pair
			{serverFields::game::WRONG_ANS_COUNT, res.wrongAnsCount},
			{serverFields::game::AVE_ANS_TIME, res.aveAnsTime}
		};
	}
}

void JsonResponsePacketSerializer::serializeRoomDataStruct(nlohmann::json* js, const RoomData& data) {
	/*
	RoomData fields: 
		unsigned int id;
		std::string name;
		unsigned int maxPlayers;
		unsigned int numOfQuestions;
		unsigned int timePerQuestion;
		bool isActive;
	format: <fieldName> : <value>
		example: "<name>": {"id": 5, "maxPlayers": 15 ....};
	*/

	(*js)[data.name] = { //all the object of the data
		{serverFields::rooms::ID, data.id}, //a pair
		{serverFields::rooms::MAX_PLAYERS, data.maxPlayers},
		{serverFields::rooms::NUM_OF_QUESTIONS, data.numOfQuestions},
		{serverFields::rooms::TIME_PER_QUSTION, data.timePerQuestion},
		{serverFields::rooms::IS_ACTIVE, data.isActive}
	};
}

void JsonResponsePacketSerializer::serializeUserStatisticsStruct(nlohmann::json* js, const UserStatistics& data) {
	/*
	UserStatistics fields:
		std::string username;
		float aveAnswerTime;
		int correctAnswersCount;
		int totalAnswersCount;
		int gamesCount;
		int score;
	format <fieldName> : <value>
		example: "<username>": {"gameCount": 8, "correctAnswersCount": 19 ...};
	*/

	(*js)[data.username] = {  //all the object of the data
		{serverFields::statistics::AVE_ANS_TIME, data.aveAnswerTime}, //a pair
		{serverFields::statistics::CORRECT_ANSWERS_COUNT, data.correctAnswersCount},
		{serverFields::statistics::ANSWER_COUNT, data.totalAnswersCount},
		{serverFields::statistics::GAME_COUNT, data.gamesCount},
		{serverFields::statistics::SCORE, data.score}
	};
}

void JsonResponsePacketSerializer::serializeGetRoomStateStruct(nlohmann::json* js, const GetRoomStateResponse& data) {
	/*
	GetRoomStateResponse:
		unsigned int status; [NOT INCLUDED IN THIS FUNCTION SERIALIZATION!]
		bool hasGameBegun;
		std::vector<LoggedUser> players;
		unsigned int numOfQuestions;
		unsigned int timePerQuestion;
	format <fieldName> : <value>
		example: "data": {"<hasGameBegun>": <hasGameBegun> ...}, "<players>": ["<player>", "<player>" ...]
	*/

	//insert data
	(*js)[serverFields::rooms::_ROOM_DATA_OBJECT] = nlohmann::json::object();
	(*js)[serverFields::rooms::_ROOM_DATA_OBJECT] = {
		{serverFields::rooms::IS_ACTIVE, data.isActive}, //a pair
		{serverFields::rooms::NUM_OF_QUESTIONS, data.numOfQuestions},
		{serverFields::rooms::TIME_PER_QUSTION, data.timePerQuestion}
	};

	//insert players
	(*js)[serverFields::general::_PLYAERS_ARR] = nlohmann::json::array();
	for (const LoggedUser& usr : data.players) {
		(*js)[serverFields::general::_PLYAERS_ARR].push_back(usr.username);
	}
}

void JsonResponsePacketSerializer::transform(unsigned char (&target)[JSON_LEN_BYTES_FROM_SERVER], unsigned int src) {

	int targetIndex = 0;

	//there is more to the src, and the array is not full
	while (src > MAX_ASCII_VALUE && targetIndex < JSON_LEN_BYTES_FROM_SERVER) {
		target[targetIndex] = MAX_ASCII_VALUE;

		//change values to there new position
		targetIndex++;
		src -= MAX_ASCII_VALUE;
	}

	//if there is more space in the target and src is still holds data
	if (src != 0 && targetIndex < JSON_LEN_BYTES_FROM_SERVER) {
		target[targetIndex] = src;

		targetIndex++;
	}

	//if there is more cells on the target that are with no value, make them 0
	int i = 0;
	for (i = targetIndex; i < JSON_LEN_BYTES_FROM_SERVER; i++) {
		target[i] = 0;
	}
}

