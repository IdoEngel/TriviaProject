#pragma once
#include <string>
#include <memory>
#include <list>

constexpr const short NUM_OF_INCORRECT_ANSWERS = 3;


using callbackP = int(*)(void*, int, char**, char**);

namespace DB_Names { //all the DB related names

	namespace UserTable { //the user table
		const std::string tName = "User";
		const std::string PK_Name = "username";
		const std::string passwordName = "password";
		const std::string emailName = "email";
	}

	namespace QuestionTable { //the question table
		const std::string tName = "Question";
		const std::string PK_Name = "ques";
		const std::string correctAns = "correctAns";
	}

	namespace IncorrectAnsTable { //the incorrect answers of a question
		const std::string tName = "IncorrectAns";
		const std::string questionKey = "Question_Id";
		const std::string incorrect1 = "incoOne";
		const std::string incorrect2 = "incoTwo";
		const std::string incorrect3 = "incoThree";
	}

	namespace StatisticsTable { //the statistics table
		const std::string tName = "Statistics";
		const std::string userKey = "User_Id";
		const std::string correctAnswers = "correctAns";
		const std::string averageAnsTime = "ave_Ans_Time";
	}

	namespace GameTable { //the game table
		const std::string tName = "Game";
		const std::string PK_Name = "Id";
		const std::string numOfQuestions = "questions_Count";
	}

	namespace KeyTable { //the tag table
		const std::string tName = "Key";
		const std::string userKey = "User_Id";
		const std::string gameKey = "Game_Id";
	}
}

/**
* @brief struct that simulates a row in the DB
*/
typedef struct User {
	User() = default;

	User(const std::string& username, const std::string& password, const std::string& email) {
		this->username = username;
		this->password = password;
		this->email = email;
	}

	std::string username;
	std::string password;
	std::string email;
} User;
using UserPass = std::shared_ptr<std::list<User>>; //what gets returned

/**
* @brief struct that simulates a row in the DB
*/
typedef struct QuestionDB {
	QuestionDB() = default;

	QuestionDB(const std::string& question, const std::string& correct,
		const std::string& inco1, const std::string& inco2, const std::string& inco3) {
		this->question = question;
		this->correct = correct;

		this->incorrect[0] = inco1;
		this->incorrect[1] = inco2;
		this->incorrect[2] = inco3;
	}


	std::string question;
	std::string incorrect[NUM_OF_INCORRECT_ANSWERS];
	std::string correct;

} QuestionDB;
using QuestionDbPass = std::shared_ptr<std::list<QuestionDB>>;

/**
* @brief struct that simulates a row in the DB
*/
typedef struct Statistics {
	Statistics() = default;

	Statistics(const std::string& username, const unsigned int& numOfCorrectAns, const float& aveAnsTime) {
		this->username = username;
		this->numOfCorrectAns = numOfCorrectAns;
		this->averageAnsTime = aveAnsTime;
	}

	std::string username;
	unsigned int numOfCorrectAns;
	float averageAnsTime;
} Statistics;
using StatisticsPass = std::shared_ptr<std::list<Statistics>>;

/**
* @brief struct that simulates a row in the DB
*/
typedef struct GameDB {
	GameDB() = default;

	GameDB(const int gameID, const int numOfQuestionsInGame) {
		this->ID = gameID;
		this->questionCount = numOfQuestionsInGame;
	}
	int ID;
	int questionCount;
} GameDB;
using GamePass = std::shared_ptr<std::list<GameDB>>;

/**
* @brief struct that contains both User and Game
*/
typedef struct Key {
	Key() = default;

	Key(const std::string& username, const std::string& password, const std::string& email,
		const int gameID, const int numOfQuestionsInGame) {
		this->user.username = username;
		this->user.password = password;
		this->user.email = email;

		this->game.ID = gameID;
		this->game.questionCount = numOfQuestionsInGame;
	}

	User user;
	GameDB game;
} Key;
using KeyPass = std::shared_ptr<std::list<Key>>;


/**
* @brief this class will contain the logic for the answers camming from the sql queries
*/
class Entity
{
public:
	~Entity();
	/**
	* @brief what the callback func will collect?
	*/
	enum class ToInsert {
		User,
		Question,
		Statistics,
		Game, 
		Key
	};

	/**
	* @brief the callback function to pass to the sql func
	* @param data external data passed (4th param)
	* @param argc num of fields
	* @param argv fields content
	* @param azColName the column name of each argv
	* @returns '0' to continue to the next row
	*/
	static int callback(void* data, int argc, char** argv, char** azColName);

	/**
	* @brief set the type of elements to collect
	* @param insert type to insert
	*/
	void setInsert(const ToInsert& insert);

	/**
	* @gets the insert type currently in the class
	* @return the insert type currently stored
	*/
	ToInsert getInsert() const;

	/**
	* @brief return the users collected
	* @returns 'std::shared_ptr<std::list<User>>' that will contain the data collected
	*/
	UserPass getUsers();

	/**
	* @brief return the question collected
	* @returns 'std::shared_ptr<std::list<Question>>' that will contain the data collected
	*/
	QuestionDbPass getQuestions();

	/**
	* @brief return the statistics collected
	* @returns 'std::shared_ptr<std::list<Statistics>>' that will contain the data collected
	*/
	StatisticsPass getStatistics();

	/**
	* @brief return the games collected
	* @returns 'std::shared_ptr<std::list<Game>>' that will contain the data collected
	*/
	GamePass getGames();

	/**
	* @brief return the keys (user & game) collected
	* @brief this if for statistic preposes, by running the right query 
	* @returns 'std::shared_ptr<std::list<Key>>' that will contain a User field and Game field
	*/
	KeyPass getKeys();
private:

	ToInsert _elemToInsert;
	std::list<User> _usersCash;
	std::list<QuestionDB> _questionsCash;
	std::list<Statistics> _statisticsCash;
	std::list<GameDB> _gameCash;
	std::list<Key> _keyCash;
};
