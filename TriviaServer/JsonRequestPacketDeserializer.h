#pragma once
#include "WSAInitializer.h"
#include <string>
#include <vector>
#include <algorithm>
#include "EnumCodes.h"
#include "json.hpp" //name space nlohmann::
#include "RequestResponseStructs.h"
#include "IRequestHandler.h"
#include "ClientConnectionException.h"


/**
* @brief this class takes a binary datatype and make it into structs cpp cna understand (byte -> struct)
*/
class JsonRequestPacketDeserializer
{
public:
	JsonRequestPacketDeserializer() = delete;
	JsonRequestPacketDeserializer(const JsonRequestPacketDeserializer&) = delete;
	JsonRequestPacketDeserializer& operator=(const JsonRequestPacketDeserializer&) = delete;

	/**
	* @brief gets a buffer datatype and get its data to return in a form cpp can understand
	* @param buffer the msg from the client
	* @returns struct that contains username and password (std::string)
	*/
	static LoginRequest deserializeLoginRequest(const Buffer& buffer);

	/**
	* @brief gets a buffer datatype and get its data to return in a form cpp can understand
	* @param buffer the msg from the client
	* @returns struct that contains username, password and email (std::string)
	*/
	static SignupRequest deserializeSignupRequest(const Buffer& buffer);

	/**
	* @brief gets a buffer datatype and get its data to return in a form cpp can understand
	* @param buffer the msg from the client
	* @returns struct that contains the username
	*/
	static LogoutRequest deserializeLogoutRequest(const Buffer& buffer);

	/**
	* @brief gets a buffer datatype and get its data to return in a form cpp can understand
	* @param buffer the msg from the client
	* @returns struct that contains the room ID the client want to get its players
	*/
	static GetPlayersInRoomRequest deserializeGetPlayersRequest(const Buffer& buffer);

	/**
	* @brief gets a buffer datatype and get its data to return in a form cpp can understand
	* @param buffer the msg from the client
	* @returns struct that contains the room ID the client want to join to
	*/
	static JoinRoomRequest deserializeJoinRoomRequest(const Buffer& buffer);

	/**
	* @brief gets a buffer datatype and gets its data to return in a form cpp can understand
	* @param buffer the msg from the client
	* @returns struct that contains all the information about the room the client want to create
	*/
	static CreateRoomRequest deserializeCreateRoomRequest(const Buffer& buffer);

	/**
	* @brief gets a buffer datatype and gets its data to return in a form cpp can understand
	* @param buffer the msg from the client
	* @returns struct that contains the username of the user
	*/
	static GetPersonalStatsRequest deserializeGetPersonalStatsRequest(const Buffer& buffer);

	/**
	* @brief gets a buffer datatype and gets its data to return in a form cpp can understand
	* @param buffer the msg from the client
	* @returns struct that contains the question ID the member chose
	*/
	static SubmitAnswerRequest deserializeSubmitAnswerRequest(const Buffer& buffer);

	/**
	* @brief gets the msg code of the buffer got from the client
	* @param buffer the buffer to find the code in
	* @returns the code of the msg
	*/
	static short getMsgCode(const Buffer& buffer);

	/**
	* @brief gets from the buffer the len of the json inside of it
	* @param buffer the buffer to find the len of the json in
	* @returns the len of the json inside
	*/
	static unsigned int sizeOfJson(const Buffer& buffer) noexcept;

	/**
	* @brief checks if the json format is valid
	* @brief if valid, second argument will contain the json
	* @param reqBuffer the full buffer from the client
	* @param js json object pointer - fill with data if format is valid
	* @returns 'true' if the format is valid
	*/
	static bool checkJsonFormat(const Buffer& reqBuffer, nlohmann::json* js);
private:

};