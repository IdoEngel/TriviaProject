#include "JsonRequestPacketDeserializer.h"

LoginRequest JsonRequestPacketDeserializer::deserializeLoginRequest(const Buffer& buffer) {
	LoginRequest loginData = { "" };
	std::string bufferStr = "";
	nlohmann::json js;

	JsonRequestPacketDeserializer::checkJsonFormat(buffer, &js);

	loginData.username = js.value(fieldNames::KEY_USERNAME,/*default value*/ "");
	loginData.password = js.value(fieldNames::KEY_PASSWORD,/*default value*/ "");

	js.clear();

	return loginData;
}

SignupRequest JsonRequestPacketDeserializer::deserializeSignupRequest(const Buffer& buffer) {
	SignupRequest signupData = { "" };
	std::string bufferStr = "";
	nlohmann::json js;

	JsonRequestPacketDeserializer::checkJsonFormat(buffer, &js);

	signupData.username = js.value(fieldNames::KEY_USERNAME, /*default value*/"");
	signupData.password = js.value(fieldNames::KEY_PASSWORD, /*default value*/"");
	signupData.email = js.value(fieldNames::KEY_EMAIL, /*default value*/"");


	js.clear();

	return signupData;
}

LogoutRequest JsonRequestPacketDeserializer::deserializeLogoutRequest(const Buffer& buffer) {
	LogoutRequest logoutData = { "" };
	nlohmann::json js;

	JsonRequestPacketDeserializer::checkJsonFormat(buffer, &js);

	logoutData.username = js.value(fieldNames::KEY_USERNAME, /*default value*/"");

	return logoutData;
}

GetPlayersInRoomRequest JsonRequestPacketDeserializer::deserializeGetPlayersRequest(const Buffer& buffer) {
	GetPlayersInRoomRequest roomID = { 0 };
	std::string bufferStr = "";
	nlohmann::json js;

	JsonRequestPacketDeserializer::checkJsonFormat(buffer, &js);

	roomID.roomID = (unsigned int)js.value(fieldNames::KEY_ROOM_ID, /*default value*/0);

	return roomID;
}

JoinRoomRequest JsonRequestPacketDeserializer::deserializeJoinRoomRequest(const Buffer& buffer) {
	JoinRoomRequest reqData = { 0 };
	std::string bufferStr = "";
	nlohmann::json js;

	JsonRequestPacketDeserializer::checkJsonFormat(buffer, &js);

	reqData.roomID = (unsigned int)js.value(fieldNames::KEY_ROOM_ID, /*default value*/0);
	reqData.username = LoggedUser(js.value(fieldNames::KEY_USERNAME, /*default value*/""));

	return reqData;
}

CreateRoomRequest JsonRequestPacketDeserializer::deserializeCreateRoomRequest(const Buffer& buffer) {
	CreateRoomRequest roomData;

	std::string bufferStr = "";
	nlohmann::json js;

	JsonRequestPacketDeserializer::checkJsonFormat(buffer, &js);

	roomData.roomName = js.value(fieldNames::KEY_ROOM_NAME, /*default value*/"");
	roomData.playerName = LoggedUser(js.value(fieldNames::KEY_USERNAME,  /*default value*/""));
	roomData.maxPlayers = js.value(fieldNames::KEY_MAX_PLAYERS, /*default value*/0);
	roomData.numOfQuestions = js.value(fieldNames::KEY_NUM_OF_QUESTIONS, /*default value*/0);
	roomData.timePerQuestion = js.value(fieldNames::KEY_TIME_PER_QUESTION, /*default value*/0);

	return roomData;
}

GetPersonalStatsRequest JsonRequestPacketDeserializer::deserializeGetPersonalStatsRequest(const Buffer& buffer) {
	GetPersonalStatsRequest user;
	nlohmann::json js;

	JsonRequestPacketDeserializer::checkJsonFormat(buffer, &js);

	user.username = LoggedUser(js.value(fieldNames::KEY_USERNAME, /*default value*/""));

	return user;
}

SubmitAnswerRequest JsonRequestPacketDeserializer::deserializeSubmitAnswerRequest(const Buffer& buffer) {
	SubmitAnswerRequest chosenAns;
	nlohmann::json js;

	JsonRequestPacketDeserializer::checkJsonFormat(buffer, &js);

	chosenAns.answerID = js.value(fieldNames::KEY_ANS_ID, /*default value*/1); //choose the first answer is the result not came

	return chosenAns;
}

short JsonRequestPacketDeserializer::getMsgCode(const Buffer& buffer) {
	short code = 0;

	code = static_cast<short>(buffer.data[0]);

	return code;
}


unsigned int JsonRequestPacketDeserializer::sizeOfJson(const Buffer& buffer) noexcept {
	unsigned int len = 0;
	unsigned char parts[CHARS_IN_INT] = { 0 };

	//get the len part of the package
	int i = 0;
	for (i = 0; i < LEN_OF_LEN_FIELD; i++) {
		parts[i] = static_cast<unsigned char>(buffer.data[i + LEN_OF_CODE_FIELD]);
	}

	//get all the parts to a single int number
	len = (
		static_cast<unsigned int>(parts[3]) |
		(static_cast<unsigned int>(parts[2]) << 8) |
		(static_cast<unsigned int>(parts[1]) << 16) |
		(static_cast<unsigned int>(parts[0]) << 24)
		);
	return len;
}


bool JsonRequestPacketDeserializer::checkJsonFormat(const Buffer& reqBuffer, nlohmann::json* js) {
	bool valid = true;

	std::string strJson = "";
	nlohmann::json bsonValidation;
	nlohmann::json strValidation;


	//check if the json is valid
	bsonValidation = nlohmann::json::from_bson(reqBuffer.data.begin() + LEN_OF_FIELDS_BEFORE_JSON, reqBuffer.data.end()
		, true, false);

	strJson = std::string(reqBuffer.data.begin() + LEN_OF_FIELDS_BEFORE_JSON, reqBuffer.data.end());
	strValidation = nlohmann::json::parse(strJson, nullptr, false);

	if (bsonValidation.type() == nlohmann::json::value_t::discarded &&
		strValidation.type() == nlohmann::json::value_t::discarded) {
		valid = false;
	}

	if (bsonValidation.type() != nlohmann::json::value_t::discarded) { //if the json is in a bson format
		*js = std::move(bsonValidation);
	}
	else if (strValidation.type() != nlohmann::json::value_t::discarded) { //if the json is in a str format
		*js = std::move(strValidation);
	}

	return valid;
}
