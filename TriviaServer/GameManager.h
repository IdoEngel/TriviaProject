#pragma once
#include <vector>
#include <memory>
#include <mutex>
#include "Game.h"
#include "Question.h"
#include "SqliteDatabase.h"

class GameManager
{
public:
	//singleton

	GameManager(const GameManager&) = delete;
	GameManager& operator=(const GameManager&) = delete;

	static GameManager& get();
	static void destroy();

	/**
	* @brief create a game inside the game manager
	* @param room the room that the game is in
	* @returns the game instance that created
	*/
	std::shared_ptr<Game> createGame(const std::shared_ptr<Room> room);

	/**
	* @brief delete a room from the manager
	*/
	void deleteGame(const int gameID);

	/**
	* @brief finds the users game and return it
	* @param username the username to find its room
	* @returns the game the user in
	*/
	std::shared_ptr<Game> findUsersGame(const std::string& username) const;
private:
	GameManager();

	static GameManager* _instance;

	IDatabase* _db;
	std::vector<std::shared_ptr<Game>> _games;

	mutable std::mutex _gamesProtect;
};

