#pragma once
#include <io.h>
#include <sstream>
#include <mutex>
#include <cmath>
#include <cstdarg>
#include "IDatabase.h"
#include "sqlite3.h"
#include "Entity.h"
#include "opentdbConnection.h"
#include "Game.h"

/**
* @brief DB access via SQLite
* @brief inherent from IDatabase
*/
class SqliteDatabase final : public IDatabase
{
public:
	SqliteDatabase(const SqliteDatabase&) = delete;
	SqliteDatabase& operator=(const SqliteDatabase&) = delete;
	//manage handle and overall db file (the file itself)

	/**
	* @brief a singleton function, replace c'tor
	*/
	static SqliteDatabase& get();
	static void destroy();

	virtual ~SqliteDatabase();

	/**
	* @brief open the db and create tables (if needed)
	* @returns 'true' if no error occurred
	*/
	virtual bool open() noexcept override;

	/**
	* @brief close the handle of the DB file
	* @returns 'true' if no error occurred
	*/
	virtual bool close() noexcept override;

	//checks

	/**
	* @brief checks if user with the given username exists
	* @param username the username to check if in the db
	* @returns 'true' if the user exists
	*/
	virtual bool doesUserExist(const std::string& username) override;

	/**
	* @brief checks if a password passed match to the username passed
	* @param username the username to check if the password is match
	* @param password the password to check if it is owned by the 'username'
	* @returns 'true' if the user 'username' exists and the 'password' owned by this 'username'
	*/
	virtual bool doesPasswordMatch(const std::string& username, const std::string& password) override;

	//queries and actions

	/**
	* @brief adds a new user to the users table
	* @param username the username to add (must be unique)
	* @param password the password of the user
	* @param email the email of the user
	* @returns 'true' if the user added to the db
	*/
	virtual bool addNewUser(const std::string& username, const std::string& password, const std::string& email) override;

	/**
	* @brief gets all the users that in the DB
	* @returns std::list of users
	*/
	virtual UserPass getUsers() override;

	/**
	* @brief get questions from the local DB
	* @param amount the number of questions to return
	* @return a std::list of the questions
	*/
	virtual QuestionDbPass getQuestions(const int amount) override;

	/**
	* @brief insert new questions from openTriviaDB
	* @brief the old questions will be deleted
	*/
	virtual void insertQuestions() override;

	/**
	* @brief gets the average answer time of a player
	* @param username the user to check its average ans time
	* @returns the a average of the time took to answer
	*/
	virtual float getPlayerAverageAnswerTime(const std::string& username) override;

	/**
	* @brief gets the number of correct answers had to the user
	* @param username the user to check its correct answer count
	* @returns the number of correct answers the user had
	*/
	virtual int getNumOfCorrectAnswers(const std::string& username) override;

	/**
	* @brief gets the total number of answers the user answered
	* @param username the user to check its question count
	* @returns the number of questions the user answered
	*/
	virtual int getNumOfTotalAnswers(const std::string& username) override;

	/**
	* @brief gets the total games had to the player
	* @param username the user to check its game count
	* @returns the number of games the user played
	*/
	virtual int getNumOfGamesOfPlayers(const std::string& username) override;

	/**
	* @brief gets the score of the user
	* @brief SCORE CALCULATION -  math::round( (getNumOfTotalAnswers() / getNumOfCorrectAnswers()) + getPlayerAverageAnswerTime() )
	* @brief if getNumOfCorrectAnswers()=0, all the score will be 0
	* @param username the user to calc its score
	* @returns the score of the user
	*/
	virtual int getPlayerScore(const std::string& username) override;

	/**
	* @brief gets the last ID that saved in the DB, increment by 1 to create the next game ID
	* @return the curr last game ID saved in the DB
	*/
	virtual int getLastGameID();

	/**
	* @brief submit a new game to the db
	* @param data the game to submit its data
	*/
	virtual void submitGameStatistics(const std::shared_ptr<Game> data) override;

	/**
	* @brief creates the DB with empty values, to keep  track of the number of rooms in the DB
	* @param data the game instance - to create the right ID
	*/
	virtual void createGameOnDB(const std::shared_ptr<Game> data) override;

private:
	SqliteDatabase();
	static SqliteDatabase* instance;
	/**
	* @brief creates the user table
	* @throw DB_Fail if the creation failed
	*/
	void createUserTable();

	/**
	* @brief creates the question table
	* @throw DB_Fail if the creation failed
	*/
	void createQuestionTable();

	/**
	* @brief creates the statistics table
	* @throw DB_Fail if the creation failed
	*/
	void createStatisticsTable();

	/**
	* @brief creates the game table
	* @throw DB_Fail if the creation failed
	*/
	void createGameTable();

	/**
	* @brief creates the key table
	* @throw DB_Fail if the creation failed
	*/
	void createKeyTable();

	/**
	* @brief checks if need to create new db or open an existing one
	* @param dbName the DB name to check 
	* @returns 'true' if need to create new DB file
	*/
	bool needToCreateDB(const std::string& dbName) const noexcept;

	/**
	* @brief run a query on the DB
	* @param query the query to run on the DB
	* @param callback (optional, default: nullptr) the function to run on each row
	* @param pass (optional, default: nullptr) the param to pass to the callback
	* @throw DB_Exception if the query fail
	*/
	void doQuery(const std::string& query, callbackP callback = nullptr, void* pass = nullptr);

	static const std::string _dbName;
	mutable std::mutex _protect;

	Entity _container;
	sqlite3* _handle;
	bool _openedSuccessfully;
};


