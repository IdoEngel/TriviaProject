#pragma once
#include <string>
#include <vector>
#include "EnumCodes.h"
#include "json.hpp" //name space nlohmann::
#include "RequestResponseStructs.h"

constexpr const bool TO_STR = true;
constexpr const bool TO_BIN = false;

constexpr const int MAX_ASCII_VALUE = 127;

/**
* @brief this class make a vector of uint8_t as an output (json -> byte)
*/
class JsonResponsePacketSerializer
{
public:
	JsonResponsePacketSerializer() = delete;
	JsonResponsePacketSerializer(const JsonResponsePacketSerializer&) = delete;
	JsonResponsePacketSerializer& operator=(const JsonResponsePacketSerializer&) = delete;

	/**
	* @brief takes a login response of the server and make it a json Buffer datatype that can be sent to client
	* @param loginData the status code of the login from the server logic
	* @param toStr make the Buffer string value or a json::bson? (use TO_STR and TO_BIN)
	* @returns a buffer (std::vector) of bytes (std::byte) that can be sent to the client without more logic
	*/
	static Buffer serializeLoginResponse(const LoginResponse& loginData, const bool toStr);

	/**
	* @brief takes a signup response of the server and make it json Buffer datatype that can be sent to the client
	* @param signupData the status code of the login from the server logic
	* @param toStr make the Buffer string value or a json::bson? (use TO_STR and TO_BIN)
	* @returns a Buffer (std::vector) of bytes (std::byte) that can be sent to the client without more logic
	*/
	static Buffer serializeSignUpResponse(const SignupResponse& signupData, const bool toStr);

	/**
	* @brief takes a error response of the server and make it a json Buffer datatype that can be sent to client
	* @param errData the error occurred while the server trying to do an action
	* @param toStr make the Buffer string value or a json::bson? (use TO_STR and TO_BIN)
	* @returns a Buffer (std::vector) of bytes (std::byte) that can be sent to the client without more logic
	*/
	static Buffer serializeErrorResponse(const ErrorResponse& errData, const bool toStr);

	/**
	* @brief takes a logout response of the server and make it a json buffer datatype that can be sent to the client
	* @param logoutData the status code of the logout from the server logic
	* @param toStr make the Buffer string value or a json::bson? (use TO_STR and TO_BIN)
	* @returns a Buffer (std::vector) of bytes (uint8_t) that can be sent to the client without more logic
	*/
	static Buffer serializeLogoutResponse(const LogoutResponse& logoutData, const bool toStr);

	/**
	* @brief takes a getRooms response of the server and make it a json buffer datatype that can be sent to the client
	* @param getRoomsData the data of the rooms from the server
	* @param toStr make the Buffer string value or a json::bson? (use TO_STR and TO_BIN)
	* @returns a Buffer (std::vector) of bytes (uint8_t) that can be sent to the client without more logic
	*/
	static Buffer serializeGetRoomsResponse(const GetRoomsResponse& getRoomsData, const bool toStr);

	/**
	* @brief takes a getPlayersInRoom response of the server and make it a json buffer datatype that can be sent to the client
	* @param getPlayersInRoomData the players in the requested room
	* @param toStr make the Buffer string value or a json::bson? (use TO_STR and TO_BIN)
	* @returns a Buffer (std::vector) of bytes (uint8_t) that can be sent to the client without more logic
	*/
	static Buffer serializeGetPlayersInRoomResponse(const GetPlayersInRoomResponse& getPlayersInRoomData, const bool toStr);

	/**
	* @brief takes a getHighScore response of the server and make it a json buffer datatype that can be sent to the client
	* @param getHighScoreData the top 5 players
	* @param toStr make the Buffer string value or a json::bson? (use TO_STR and TO_BIN)
	* @returns a Buffer (std::vector) of bytes (uint8_t) that can be sent to the client without more logic
	*/
	static Buffer serializeGetHighScoreResponse(const GetHighScoreResponse& getHighScoreData, const bool toStr);

	/**
	* @brief takes a getPersonalStats response of the server and make it a json buffer datatype that can be sent to the client
	* @param getPersonalStatsData the personal statistics data
	* @param toStr make the Buffer string value or a json::bson? (use TO_STR and TO_BIN)
	* @returns a Buffer (std::vector) of type (uint8_t) that can be sent to the client without more logic
	*/
	static Buffer serializeGetPersonalStatsResponse(const GetPersonalStatsResponse& getPersonalStatsData, const bool toStr);

	/**
	* @brief takes a JoinRoom response of the server and make it a json buffer datatype that can be sent to the client
	* @param joinRoomData the status code of joining a room
	* @param toStr make the Buffer string value or a json::bson? (use TO_STR and TO_BIN)
	* @returns a Buffer (std::vector) of type (uint8_t) that can be sent to the client without more logic
	*/
	static Buffer serializeJoinRoomResponse(const JoinRoomResponse& joinRoomData, const bool toStr);

	/**
	* @brief take a CreateRoom response of the server and make it a json buffer datatype that can be sent to the client
	* @param createRoomData the status code of creating room
	* @param toStr make the Buffer string value or a json::bson? (use TO_STR and TO_BIN)
	* @returns a Buffer (std::vector) of type (uint8_t) that can be sent to the client without more logic
	*/
	static Buffer serializeCreateRoomResponse(const CreateRoomResponse& createRoomData, const bool toStr);

	/**
	* @brief take a CloseRoom response of the server and make it a json buffer datatype that can be sent to the client
	* @param closeRoomData the status code of closing room
	* @param toStr make the Buffer string value or a json::bson? (use TO_STR and TO_BIN)
	* @returns a Buffer (std::vector) of type (uint8_t) that can be sent to the client without more logic
	*/
	static Buffer serializeCloseRoomResponse(const CloseRoomResponse& closeRoomData, const bool toStr);

	/**
	* @brief take a StartGame response of the server and make it a json buffer datatype that can be sent to the client
	* @param startGameData the status code of starting game
	* @param toStr make the Buffer string value or a json::bson? (use TO_STR and TO_BIN)
	* @returns a Buffer (std::vector) of type (uint8_t) that can be sent to the client without more logic
	*/
	static Buffer serializeStartGameResponse(const StartGameResponse& startGameData, const bool toStr);

	/**
	* @brief take a GetRoomState response of the server and make it a json buffer datatype that can be sent to the client
	* @param getRoomStateData the data of the state of the room
	* @param toStr make the Buffer string value or a json::bson? (use TO_STR and TO_BIN)
	* @returns a Buffer (std::vector) of type (uint8_t) that can be sent to the client without more logic
	*/
	static Buffer serializeGetRoomStateResponse(const GetRoomStateResponse& getRoomStateData, const bool toStr);

	/**
	* @brief take a LeaveRoom response of the server and make it a json buffer datatype that can be sent to the client
	* @param leaveRoomData status code of leaving room
	* @param toStr make the Buffer string value or a json::bson? (use TO_STR and TO_BIN)
	* @returns a Buffer (std::vector) of type (uint8_t) that can be sent to the client without more logic
	*/
	static Buffer serializeLeaveRoomResponse(const LeaveRoomResponse& leaveRoomData, const bool toStr);

	/**
	* @brief take a NotifyAdminAboutLeftMember response of the server and make it a json buffer datatype that can be sent to the client
	* @param data the member that left
	* @param toStr make the Buffer string value or a json::bson? (use TO_STR and TO_BIN)
	* @returns a Buffer (std::vector) of type (uint8_t) that can be sent to the client without more logic
	*/
	static Buffer serializeNotifyAdminAboutLeftMemberResponse(const NotifyAdminAboutLeftMember& data, const bool toStr);

	/**
	* @brief take a LeaveGame response of the server and make it a json buffer datatype that can be sent to the client
	* @param data the the response of the leaving request
	* @param toStr make the buffer string value of json::bson? (use TO_STR and TO_BIN)
	* @returns a buffer (std::vector) of type (uint8_t) that can be sent to the client without more logic
	*/
	static Buffer serializeLeaveGameResponse(const LeaveGameResponse& data, const bool toStr);

	/**
	* @brief take a GetQuestion response of the server and make it a json buffer datatype that can be sent to the client
	* @param data the returns to the client the next question and the possible answers
	* @param toStr make the Buffer string value or a json::bson? (use TO_STR and TO_BIN)
	* @returns a buffer (std::vector) of type (uint8_t) that can be sent to the client without more logic
	*/
	static Buffer serializeGetQuestionResponse(const GetQuestionResponse& data, const bool toStr);

	/**
	* @brief take a SubmitAnswer response of the server and make it a json buffer datatype that can be sent to the client
	* @param data the response after member sent its answer of the question
	* @param toStr make the Buffer string value of a json::bson? (use TO_STR and TO_BIN)
	* @returns a buffer (std::vector) of type (uint8_t) that can be sent to the client without more logic
	*/
	static Buffer serializeSubmitAnswerResponse(const SubmitAnswerResponse& data, const bool toStr);

	/**
	* @brief take a GetGameResults response of the server and make it a json buffer datatype that can be sent to the client
	* @param data the response of requesting a game result
	* @param toStr make the Buffer string value of a json::bson? (use TO_STR and TO_BIN)
	* @returns a buffer (std::vector) of type (uint8_t) that can be sent to the client without more logic
	*/
	static Buffer serializeGetGameResultsResponse(const GetGameResultsResponse& data, const bool toStr);

	/**
	* @brief returns the buffer datatype that contains the data from the input
	* @param src the place to take the data to the buffer
	* @param len the len of the char sequence
	* @returns Buffer datatype with the same values of 'src'
	*/
	static Buffer toBytesBuffer(char* src, const int len);
private:

	/**
	* @brief append to the json in the position transferred the data of the RoomData struct
	* @param js the json to append to (on the spot it transferred)
	* @param data the data to append
	*/
	static void serializeRoomDataStruct(nlohmann::json* js, const RoomData& data);

	/**
	* @brief append to the json in the position transferred the data of the UserStatistics struct
	* @param js the json to append to (on the spot it transferred)
	* @param data the data to append
	*/
	static void serializeUserStatisticsStruct(nlohmann::json* js, const UserStatistics& data);

	/**
	* @brief append to the json in the position transferred the data of the GetRoomStateResponse struct
	* @param js the json to append to (on the spot it transferred)
	* @param data the data to append
	*/
	static void serializeGetRoomStateStruct(nlohmann::json* js, const GetRoomStateResponse& data);

	/**
	* @brief append to the json in the position transferred the data of the GetQuestionResponse struct
	* @param js the json append to (on the spot transferred)
	* @param data the data to append
	*/
	static void serializeMapOfAnswersToAQuestion(nlohmann::json* js, const GetQuestionResponse& data);

	/**
	* @brief append to the json in the position transferred the data of the std::vector<PlayerResults>
	* @param js the json to append (on the spot transferred)
	* @param data the data to append
	*/
	static void serializeVectorOfPlayerResult(nlohmann::json* js, const std::vector<PlayerResults>& data);

	/**
	* @brief adds to the buffer all the data collected
	* @param bf the buffer to append to
	* @param js the json to append
	* @param jsSize the size of the json
	* @param status the status code from the server logic
	*/
	static void createBuffer(Buffer* bf, const nlohmann::json& js, const uint8_t status, const bool toStr);

	/**
	* @brief transform the src to array of chars, that any of them is not bigger then 127
	* @param target the answer will be come from here (EACH INDEX WONT BE LARGER THEN 127)
	* @param src the number to convert into chars
	*/
	static void transform(unsigned char(&target)[JSON_LEN_BYTES_FROM_SERVER], unsigned int src);
};

