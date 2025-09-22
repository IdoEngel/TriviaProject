#pragma once

/**
* @brief server codes that sent to the client
*/
enum class Codes {
	GENERAL_ERROR = 9,

	SIGN_UP_SUCCESS = 10,
	LOG_IN_SUCCESS = 11,
	LOG_OUT_SUCCESS = 12,
	SIGN_UP_FAIL = 13,
	LOG_IN_FAIL = 14,
	LOG_OUT_FAIL = 15,

	GET_ROOMS_SUCCESS = 16,
	GET_PLAYERS_IN_ROOM_SUCCESS = 17,
	GET_HIGN_SCORE_TOP_FIVE_SUCCESS = 18,
	GET_PERSONAL_STATS_SUCCESS = 19,
	JOIN_ROOM_SUCCESS = 20,
	CREATE_ROOM_SUCCESS = 21,

	CLOSE_ROOM_SUCCESS = 22,
	START_GAME_SUCCESS = 23,
	GET_ROOM_STATE_SUCCESS = 24,
	LEAVE_ROOM_SUCCESS = 25,
	_NOTIFY_CLOSED_ROOM = 26,
	_NOTIFY_GAME_STARTED = 27,
	_NOTIFY_ADMIN_MEMBER_LEFT = 28,

	LEAVE_GAME_SUCCESS = 29,
	GET_QUESTION_SUCCESS = 30,
	SUBMIT_ANS_SUCCESS = 31,
	GET_ROOM_RESULTS_SUCCESS = 32,
	GET_ROOM_RESULTS_FAIL = 33,
};

/**
* @brief client codes sent to the server
*/
enum class ClientCodes {
	GENERAL_ERROR = 50,
	SIGN_UP_REQUEST = 51,
	LOG_IN_REQUEST = 52,
	LOG_OUT_REQUEST = 53,

	PLAYERS_IN_ROOM_REQUEST = 54,
	JOIN_ROOM_REQUEST = 55,
	CREATE_ROOM_REQUEST = 56,
	GET_HIGH_SCORE_REQUEST = 57,
	GET_PRSONAL_STAT_REQUEST = 58,
	GET_ROOMS_REQUEST = 59,

	CLOSE_ROOM_REQUEST = 60,
	START_GAME_REQUEST = 61,
	GET_ROOM_STATE_REQUEST = 62,
	LEAVE_ROOM_REQUEST = 63,
	_NOTIFY_CLOSED_ROOM_sts = 64, //server-to-server code
	_NOTIFY_GAME_STARTED_sts = 65, //server-to-server code
	_NOTIFY_ADMIN_THAT_MEMBER_LEFT_sts = 66, //server-t-server code

	LEAVE_GAME_REQUEST = 67,
	GET_QUESTION_REQUEST = 68,
	SUBMIT_ANS_REQUEST = 69,
	GET_GAME_RESULTS_REQUEST = 70,
};

//operators to compare the datatype without casting inside the 'if' statement, one-line funcs


constexpr inline bool operator==(Codes const& left, unsigned int const& right);
constexpr inline bool operator==(unsigned int const& left, Codes const& right);

constexpr inline bool operator==(ClientCodes const& left, unsigned int const& right);
constexpr inline bool operator==(unsigned int const& left, ClientCodes const& right);
