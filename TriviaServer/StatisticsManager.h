#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include "SqliteDatabase.h"
#include "RequestResponseStructs.h"

constexpr int GET_TOP = 5;

class StatisticsManager
{
public:
	//singleton
	static StatisticsManager& get();
	static void destroy();
	StatisticsManager(const StatisticsManager&) = delete;
	StatisticsManager& operator=(const StatisticsManager&) = delete;

	/**
	* @brief get the statistics of a user provided
	* @param username the user to get its statistics
	*/
	UserStatistics getUserStatistics(const std::string& username);

	/**
	* @brief get the top 5 best players, in a vector
	* @brief if less then 5 users are in the DB, return all in order from top to bottom
	* @returns vector of the top 5 players, if there are
	*/
	std::vector<UserStatistics> getHighScore();
private:
	//singleton
	StatisticsManager();
	static StatisticsManager* _instance;

	IDatabase& _db;
};

