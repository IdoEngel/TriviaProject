#pragma once
#include <vector>
#include <mutex>
#include "SqliteDatabase.h"
#include "RequestResponseStructs.h"

/**
* @brief manages the users that logged in, or want to be logged-in
*/
class LoginManager
{
public:
	LoginManager(const LoginManager&) = delete;
	LoginManager& operator=(const LoginManager&) = delete;

	/**
	* @brief a singleton function, replace c'tor
	*/
	static LoginManager& get();
	static void destroy();

	/**
	* @brief create an account for a user
	* @param username new user's username
	* @param password new user's password
	* @param email new user's email
	* @return 'true' if the user added to the DB
	*/
	bool signup(const std::string& username, const std::string& password, const std::string& email);

	/**
	* @brief add the user as a currently logged in
	* @param username user's username
	* @param password user's password
	* @returns 'true' if the login happened
	*/
	bool login(const std::string& username, const std::string& password);

	/**
	* @brief log out the user from the vector of the logged ones
	* @brief if the user does not exists or not logged in - do nothing
	* @param username the username to log out
	*/
	void logout(const std::string& username);

	/**
	* @brief check if the username provided is in the vector of logged users
	* @param username the username to check
	* @returns 'true' if the user logged already
	*/
	bool isLoggedIn(const std::string& username);

private:

	void insertToLoggedUsers(const std::string& user);

	void deleteFromLoggedUser(const std::vector<LoggedUser>::iterator& user);
	static LoginManager* instance;

	LoginManager();
	~LoginManager();
	IDatabase& _db;
	std::vector<LoggedUser> _loggedUsers;

	std::mutex _loggedUsersLock;
};

