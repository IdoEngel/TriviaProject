#pragma once
#include <memory>
#include <string>
#include <vector>
#include "DB_Exception.h"
#include "Console.h"
#include "Entity.h"

class Game;
/**
* @brief INTERFACE - only virtual funcs
* @brief functions all the classes of type Handler will need to create
* @note the classes that will inherent from this will be able to create more then the funcs in here
*/
class IDatabase {
public:
	//manage handle and overall db file (the file itself)
	virtual bool open() noexcept = 0;
	virtual bool close() noexcept = 0;

	//checks
	virtual bool doesUserExist(const std::string& username) = 0;
	virtual bool doesPasswordMatch(const std::string& username, const std::string& password) = 0;

	virtual float getPlayerAverageAnswerTime(const std::string& username) = 0;
	virtual int getNumOfCorrectAnswers(const std::string& username) = 0;
	virtual int getNumOfTotalAnswers(const std::string& username) = 0;
	virtual int getNumOfGamesOfPlayers(const std::string& username) = 0;
	virtual int getPlayerScore(const std::string& username) = 0;
	virtual int getLastGameID() = 0;

	//queries and actions
	virtual bool addNewUser(const std::string& username, const std::string& password, const std::string& email) = 0;
	virtual UserPass getUsers() = 0;

	virtual QuestionDbPass getQuestions(const int amount) = 0;
	virtual void insertQuestions() = 0;
	virtual void submitGameStatistics(const std::shared_ptr<Game> data) = 0;
	virtual void createGameOnDB(const std::shared_ptr<Game> data) = 0;


protected:
};
