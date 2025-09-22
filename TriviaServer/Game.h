#pragma once
#include <string>
#include <algorithm>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <iterator>
#include "RequestResponseStructs.h"
#include "Question.h"
#include "IDatabase.h"
#include "Room.h"

/**
* @brief manages all the curr statistics of the game, and keeps copy of all the questions that will pop out throw out it
*/
class Game
{
public:
	Game() = default;

	/**
	* @brief CONSTRACTOR
	* @param gameID the id of the game to create
	*/
	Game(const int gameID);

	//copy-constractor
	Game(const Game& other);

	/**
	* @brief remove the user from the players in the game
	* @param username the username to remove
	*/
	void removePlayer(const LoggedUser& username);

	/**
	* @brief adds one user to the game's players
	* @param username the user to add
	*/
	void addPlayer(const LoggedUser& username);

	/**
	* @brief adds all the players in the given room
	* @param room the room to add all its users
	*/
	void addPlayers(const std::shared_ptr<Room> room);

	/**
	* @brief gets the next question for the user
	* @param username the username to get its next question
	* @return an optional type of the class member 'Question'
	*/
	std::optional<std::shared_ptr<Question>> getQuestionForUser(const LoggedUser& username);

	/**
	* @brief get the user and the ID of the answer, and update the statistic data of the client in the class
	* @brief increasing the question count
	* @param user the user that submitted the answer
	* @param ansID the ID of the answer that the user chose 
	* @param timeTook the time that passed from the getting of the question on the client side (by the Handler)
	*/
	void submitAnswer(const LoggedUser& user, const int ansID, const int timeTook);

	/**
	* @brief GETTER
	* @returns the ID of the game
	*/
	int getGameID() const;

	/**
	* @brief GETTER
	* @returns the number of questions
	*/
	unsigned int getNumberOfQuestions() const;

	/**
	* @brief GETTER
	* @returns the number of members
	*/
	unsigned int getNumberOfMembers() const;

	/**
	* @brief GETTER - deep copy
	* @returns all the players and their statistics 
	*/
	std::map<LoggedUser, GameData> getPlayers() const;

	/**
	* @brief add all the questions passed to the class containers - deep copy
	* @param questions the questions to add to the class
	*/
	void addQuestions(QuestionDbPass questions);

	/**
	* @brief check if all players finished the game and can commit statistics to the DB
	* @param db the database class to update if game finished
	* @returns 'true' if the game ended and DB updated
	*/
	bool checkForEndGame(IDatabase* db);


private:

	int _gameID;
	unsigned int _numberOfQuestions = 0;

	std::map<LoggedUser, GameData> _players;
	mutable std::mutex _playersProtect;

	std::vector<std::shared_ptr<Question>> _questions;
	std::mutex _questionsProtect;
};

