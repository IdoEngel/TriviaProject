#pragma once
#include <ctime>
#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <map>
#include <optional>
#include <chrono>
#include "EnumCodes.h"

constexpr const short MAX_SIZE_CHAR = 255;
constexpr const short CHARS_IN_INT = 4;
constexpr const short JSON_LEN_BYTES_FROM_SERVER = 8;

constexpr const short LEN_OF_CODE_FIELD = 1;
constexpr const short LEN_OF_LEN_FIELD = 4;
constexpr const short LEN_OF_FIELDS_BEFORE_JSON = LEN_OF_CODE_FIELD + LEN_OF_LEN_FIELD;

constexpr const size_t MIN_VECTOR_SIZE = 20;

constexpr const short NUM_OF_QUESTIONS_IN_DB = 35;

constexpr const int INVALID_MSG_ID = 5; //for sending to client if tried to submit answer after finished all questions

 namespace serverFields {

	namespace general {
		const std::string ERR_MSG = "msg"; //general error

		const std::string CODE = "code"; //general code

		//players in room data & top 5 high score 
		const std::string _PLYAERS_ARR = "players"; //players in room data & top 5 high score & results of games
	}

	namespace rooms {
		const std::string _ROOMS_ARR = "rooms"; //room data
		const std::string ID = "id"; //room data
		const std::string MAX_PLAYERS = "maxPlayers"; //room data
		const std::string NUM_OF_QUESTIONS = "questionCount"; //room data
		const std::string TIME_PER_QUSTION = "questionTimer"; //room data
		const std::string IS_ACTIVE = "isActive"; //room data
		const std::string _ROOM_DATA_OBJECT = "data"; //room data
		const std::string LEFT_PLAYER = "leftMember"; //room data
	}

	//statistics data
	namespace statistics {
		const std::string AVE_ANS_TIME = "aveAnswerTime"; //statistics data
		const std::string CORRECT_ANSWERS_COUNT = "correctAnswersCount"; //statistics data
		const std::string ANSWER_COUNT = "totalAnswersCount"; //statistics data
		const std::string GAME_COUNT = "gameCount"; //statistics data
		const std::string SCORE = "score"; //statistics data
	}

	namespace game { //game data
		const std::string QUESTION = "question";
		const std::string _ANSWERS_ARR = "answers"; //object as key-value [ans number-ans]
		const std::string CORRECT_ANS_ID = "correctAnsID";
		const std::string IS_SUCCESSFUL = "isSuccessful"; //is the call to GameResult is passible (is game over?)

		const std::string CORRECT_ANS_COUNT = "correctAnsCount";
		const std::string WRONG_ANS_COUNT = "wrongAnsCount";
		const std::string AVE_ANS_TIME = "aveAnsTime";
	}

}

namespace fieldNames { //names of fields on the protocol - from the client
	//login, signup logout - from client
	const std::string KEY_USERNAME = "username"; //login, signup logout
	const std::string KEY_PASSWORD = "password"; //login, signup logout
	const std::string KEY_EMAIL = "email"; //login, signup logout

	const std::string KEY_ROOM_ID = "id"; //room requests
	const std::string KEY_ROOM_NAME = "name"; //room requests
	const std::string KEY_MAX_PLAYERS = "maxPlayers"; //room requests
	const std::string KEY_NUM_OF_QUESTIONS = "numOfQuestions"; //room requests
	const std::string KEY_TIME_PER_QUESTION = "timePerQuestion"; //room requests

	const std::string KEY_ANS_ID = "ansID"; //game requests
}

namespace fieldCount { //the number of fields in each request - from the client
	constexpr int SIGN_UP = 3;
	constexpr int LOG_IN = 2;
	constexpr int LOG_OUT = 1;
	constexpr int PLAYERS_IN_ROOM = 1;
	constexpr int JOIN_ROOM = 2;
	constexpr int CREATE_ROOM = 5;
	constexpr int GET_PERSONAL_STATS = 1;
	constexpr int SUBMIT_ANSWER = 1;
}

/**
* @brief buffer of serialized data to send to the client or data got from it
* @ data : std::vector<uint8_t>
*/
typedef struct Buffer {
	std::vector<uint8_t> data;

	Buffer() {
		this->data.reserve(MIN_VECTOR_SIZE); //20 elements for the creation of the vector - less allocations later
	}

	Buffer(Buffer&& other) noexcept {
		this->data = std::move(other.data);
	}

	Buffer(const Buffer& other) {
		for (auto& item : other.data) {
			this->data.push_back(item);
		}
	}

	~Buffer() {
		this->data.resize(0);
		this->data.clear();
	}

	Buffer& operator=(const Buffer& other) {
		this->data.clear();
		this->data.resize(0);

		this->data.resize(other.data.size());
		std::copy(other.data.begin(), other.data.end(), this->data.begin());

		return *this;
	}

} Buffer;

/**
* @brief info of the request that coming
* @brief id : size_t
* @brief receivalTime : time_t
* @brief buffer : Buffer (std::vector<uint8_t>)
*/
typedef struct RequestInfo {
	size_t id; //the ID of the request
	std::chrono::steady_clock::time_point receivalTime; //the time the request received
	Buffer buffer; //buffer of more info

	RequestInfo(size_t ID, const Buffer& buff) {
		this->id = ID;
		receivalTime = std::chrono::steady_clock::now();
		this->buffer = buff;
	}

	~RequestInfo() {
		this->buffer.data.resize(0);
		this->buffer.data.clear();
	}
} RequestInfo;

/**
* @brief logged-in user data
* @brief username : std::string
*/
typedef struct LoggedUser {
	LoggedUser() = default;
	LoggedUser(const std::string& user) {
		username = user;
	}

	friend bool operator==(LoggedUser const& left, LoggedUser const& right) {
		return (left.username == right.username);
	}

	friend bool operator<(LoggedUser const& left, LoggedUser const& right) {
		return (left.username < right.username);
	}

	std::string username;
} LoggedUser;

/**
* @brief the struct holds all the data of the user while in a game
* @brief currQuestion : std::string
* @brief correctAnsCount : unsigned int
* @brief wrongAnsCount : unsigned int
* @brief aveAnsTime : float
*/
typedef struct GameData {
	GameData() : currQuestion(""), correctAnsCount(0), wrongAnsCount(0), questionNumber(0), aveAnsTime(0) {}

	std::string currQuestion;
	unsigned int correctAnsCount;
	unsigned int wrongAnsCount;
	unsigned int questionNumber;
	float aveAnsTime;
} GameData;

/**
* @brief status code of log in
* @brief status : unsigned int
*/
typedef struct LoginResponse {
	unsigned int status;
} LoginResponse;

/**
* @brief status code of sign up
* @brief status : unsigned int
*/
typedef struct SignupResponse {
	unsigned int status;
} SignupResponse;

/**
* @brief msg of an error - if occurs
* @brief message : std::string
*/
typedef struct ErrorResponse {
	std::string message;
} ErrorResponse;

/**
* @brief username and password of the client
* @brief username : std::string
* @brief password : std::string
*/
typedef struct LoginRequest {
	std::string username;
	std::string password;
} LoginRequest;

/**
* @brief username, password and email of the client
* @brief username : std::string
* @brief password : std::string
* @brief email : std::string
*/
typedef struct SignupRequest {
	std::string username;
	std::string password;
	std::string email;
} SignupRequest;

/**
* @brief the username to logout (the user sent the msg)
* @brief username : std::string
*/
typedef struct LogoutRequest {
	std::string username;
} LogoutRequest;

/**
* @brief room meta-data and DB rows names and types
* @brief id : const unsigned int
* @brief name : std::string
* @brief maxPlayers : unsigned int
* @brief numOfQuestions : unsigned int
* @brief timePerQuestion : unsigned int
* @brief isActive : bool
*/
typedef struct RoomData {
	unsigned int id;
	std::string name;
	unsigned int maxPlayers;
	unsigned int numOfQuestions;
	unsigned int timePerQuestion;
	bool isActive; //room state/status

	//for emplace-back in STL containers
	RoomData() : id(0), name(""), maxPlayers(0), numOfQuestions(0), timePerQuestion(0), isActive(false) {}

	//for emplace-back in STL containers
	RoomData(const unsigned int id, const std::string& name,
		const unsigned int maxPlayers, const unsigned int numOfQuestions,
		const unsigned int timePerQuestion, const bool isActive)
	{
		this->id = id;
		this->isActive = isActive;
		this->maxPlayers = maxPlayers;
		this->name = name;
		this->numOfQuestions = numOfQuestions;
		this->timePerQuestion = timePerQuestion;
	}
} RoomData;

/**
* @brief all the statistics of the user, in one place
*/
typedef struct UserStatistics {
	UserStatistics() = default;

	friend std::ostream& operator<<(std::ostream& os, UserStatistics const& value) { //for debagging
		os << value.username << ": " << "got right " << value.correctAnswersCount << " questions of " << value.totalAnswersCount;
		os << ". Did it in " << value.gamesCount << " games, and got score of " << value.score << ", in average answer time of ";
		os << value.aveAnswerTime;

		return os;
	}

	std::string username = "";
	float aveAnswerTime = 0;
	int correctAnswersCount = 0;
	int totalAnswersCount = 0;
	int gamesCount = 0;
	int score = 0;
} UserStatistics;

/**
* @brief logout data
* @brief status : unsigned int
*/
typedef struct LogoutResponse {
	unsigned int status;
} LogoutResponse;

/**
* @brief all the rooms currently on the server
* @brief status : unsigned int
* @brief rooms : std::vector<RoomData>
*/
typedef struct GetRoomsResponse {
	unsigned int status;
	std::vector<RoomData> rooms;
} GetRoomsResponse;

/**
* @brief vector of players, represents players in a requested room
* @brief status : unsigned int
* @brief players : std::vector<LoggedUser>
*/
typedef struct GetPlayersInRoomResponse {
	unsigned int status;
	std::vector<LoggedUser> players;
} GetPlayersInRoomResponse;

/**
* @brief a response of the StatisticManager getHighScore() func
* @brief status : unsigned int
* @brief statistics : std::vector<UserStatistics> - 
*/
typedef struct GetHighScoreResponse {
	unsigned int status;
	std::vector<UserStatistics> statistics;
} getHighScoreResponse;

/**
* @brief a response of the StatisticManager getUserStatistics(const std::string& username)
* @brief status : unsigned int
* @brief statistics : UserStatistics
*/
typedef struct GetPersonalStatsResponse {
	unsigned int status;
	UserStatistics statistics;
} getPersonalStatsResponse;

/**
* @brief status of joining a room
* @brief status : unsigned int
*/
typedef struct JoinRoomResponse {
	unsigned int status;
} JoinRoomResponse;

/**
* @brief status of creating room
* @brief status : unsigned int
* @brief roomID : unsigned int
*/
typedef struct CreateRoomResponse {
	unsigned int status;
	unsigned int roomID;
} CreateRoomResponse;

/**
* @brief request from the client to get players in a room, by ID
* @brief roomID : unsigned int
*/
typedef struct GetPlayersInRoomRequest {
	unsigned int roomID;
} GetPlayersInRoomRequest;

/**
* @brief request from the client to join a room, by ID
* @brief roomID : unsigned int
* @brief username : LoggedUser
*/
typedef struct JoinRoomRequest {
	unsigned int roomID;
	LoggedUser username;
} JoinRoomRequest;

/**
* @brief request for creating room
* @brief roomName : std::string
* @brief playerName : LoggedUser
* @brief maxPlayers : unsigned int
* @brief numOfQuestions : unsigned int
* @brief timePerQuestion : unsigned int
*/
typedef struct CreateRoomRequest {
	std::string roomName;
	LoggedUser playerName;
	unsigned int maxPlayers;
	unsigned int numOfQuestions;
	unsigned int timePerQuestion;
} CreateRoomRequest;

/**
* @brief the client need to send its username to the server to get the stats
* @brief username : LoggedUser
*/
typedef struct GetPersonalStatsRequest {
	LoggedUser username;
} GetPersonalStatsRequest;

/**
* @brief the server sends after the room closed
* @brief status : unsigned int
*/
typedef struct CloseRoomResponse {
	unsigned int status;
} CloseRoomResponse;

/**
* @brief the server sends after the game started
* @brief status : unsigned int
*/
typedef struct StartGameResponse {
	unsigned int status;
} StartGameResponse;

/**
* @brief from the server, get room state
* @brief status : unsigned int
* @brief isActive : bool
* @brief players : std::vector<LoggedUser>
* @brief numOfQuestions : unsigned int 
* @brief timePerQuestion : unsigned int
*/
typedef struct GetRoomStateResponse {
	unsigned int status;
	bool isActive;
	std::vector<LoggedUser> players;
	unsigned int numOfQuestions;
	unsigned int timePerQuestion;
} GetRoomStateResponse;

/**
* @brief from the server, for leaving a room
* @brief status : unsigned int
*/
typedef struct LeaveRoomResponse {
	unsigned int status;
} LeaveRoomResponse;

/**
* @brief from the server, notify the admin of the room that one of the members left
* @brief status : unsigned int
* @brief memberUsername : std::string
*/
typedef struct NotifyAdminAboutLeftMember {
	unsigned int status;
	std::string memberUsername;
} NotifyAdminAboutLeftMember;

/**
* @brief from the server, after user tried to leave a game
* @brief status : unsigned int
*/
typedef struct LeaveGameResponse {
	unsigned int status;
} LeaveGameResponse;

/**
* @brief from the server, after user wanted to get the next question
* @brief status : unsigned int
* @brief question : std::string
* @brief answers : std::map<unsigned int, std::string>
*/
typedef struct GetQuestionResponse {
	unsigned int status;
	std::string question;
	std::map<unsigned int, std::string> answers;
} GetQuestionResponse;

/**
* @brief from the server, after getting the choice of the user
* @brief status : unsigned int
* @brief correctAnswerId : unsigned int
*/
typedef struct SubmitAnswerResponse {
	unsigned int status;
	unsigned int correctAnswerId;
} SubmitAnswerResponse;

/**
* @brief a sub-struct, not in direct use
* @brief collects all the statistics of all the members in the game, for the current game
*/
typedef struct PlayerResults {
	PlayerResults() : username(""), correctAnsCount(0), wrongAnsCount(0), aveAnsTime(0) {};

	/**
	* @brief change all the fields of the struct after creation
	* @param _username the new username
	* @param _correctAnsCount the new correct answers counter
	* @param _wrongAnsCount the new wrong answer counter
	* @param _aveAnsTime the new average answer time 
	*/
	void change(const std::string& _username, const unsigned int _correctAnsCount, const unsigned int _wrongAnsCount, 
		const float _aveAnsTime) {

		this->username = _username;
		this->correctAnsCount = _correctAnsCount;
		this->wrongAnsCount = _wrongAnsCount;
		this->aveAnsTime = _aveAnsTime;
	}

	std::string username;
	unsigned int correctAnsCount;
	unsigned int wrongAnsCount;
	float aveAnsTime;
} PlayerResults;

/**
* @brief from the server, after game ended when user asks for results
* @brief status : unsigned int
* @brief successful : const bool
* @brief results : std::vector<PlayerResults>
*/
typedef struct GetGameResultsResponse {
	/**
	* @brief gets the number of players in room to pre-allocate memo in vector
	* @brief get throw the indexes, all the elements already created
	* @param amount the number of members in the room
	* @param isSuccessful is the game over and get results? [default=true]
	*/
	GetGameResultsResponse(const unsigned int amount, const bool isSuccessful=true) 
	: successful(isSuccessful) {

		this->status = (isSuccessful ? 
			static_cast<int>(Codes::GET_ROOM_RESULTS_SUCCESS) : static_cast<int>(Codes::GET_ROOM_RESULTS_FAIL));

		if (isSuccessful) { //only if the call was successful (the game ended)
			this->results.resize(amount);
		}
	}

	//update all the results from the game - send  game->getPlayers() return type
	void updateAll(const std::map<LoggedUser, GameData>& allData) {

		std::vector<PlayerResults>::iterator itUpdate = this->results.begin();
		std::map<LoggedUser, GameData>::const_iterator itData = allData.begin();

		//for every item in the results (created in the constractor)
		for (itUpdate = itUpdate; itUpdate != this->results.end(); ++itUpdate) {
			itUpdate->change(itData->first.username, itData->second.correctAnsCount, itData->second.wrongAnsCount,
				itData->second.aveAnsTime);

			++itData; //next iterator to get its data
		}
	}

	unsigned int status;
	const bool successful;
	std::vector<PlayerResults> results; //access with indexes, no need to create
} GetGameResultsResponse;

/**
* @brief get the answer choice from the user, as the number of the question
* @brief answerID : unsigned int
*/
typedef struct SubmitAnswerRequest {
	unsigned int answerID;
} SubmitAnswerRequest;

