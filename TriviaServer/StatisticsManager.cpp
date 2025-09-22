#include "StatisticsManager.h"

StatisticsManager* StatisticsManager::_instance = nullptr;

StatisticsManager::StatisticsManager() : _db(SqliteDatabase::get()) { }

StatisticsManager& StatisticsManager::get() {
	if (StatisticsManager::_instance == nullptr) {
		StatisticsManager::_instance = new StatisticsManager();
	}

	return *StatisticsManager::_instance;
}

void StatisticsManager::destroy() {
	delete StatisticsManager::_instance;
	StatisticsManager::_instance = nullptr;
}

UserStatistics StatisticsManager::getUserStatistics(const std::string& username) {
	UserStatistics userData;

	userData.username = username;
	userData.aveAnswerTime = this->_db.getPlayerAverageAnswerTime(username);
	userData.correctAnswersCount = this->_db.getNumOfCorrectAnswers(username);
	userData.gamesCount = this->_db.getNumOfGamesOfPlayers(username);
	userData.score = this->_db.getPlayerScore(username);
	userData.totalAnswersCount = this->_db.getNumOfTotalAnswers(username);

	return userData;
}

std::vector<UserStatistics> StatisticsManager::getHighScore() {
	std::vector<UserStatistics> all;
	std::vector<UserStatistics> topFive;
	UserPass allUsers;
	int limit = 0;

	allUsers = this->_db.getUsers();
	all.resize(allUsers->size()); //all and allUsers the same size

	//fill in the data of all the users
	//track both containers
	std::vector<UserStatistics>::iterator allIt = all.begin();
	std::list<User>::const_iterator usersIt = allUsers->begin();

	for (usersIt = usersIt; usersIt != allUsers->end(); ++usersIt, ++allIt) { //iterete both at the same time
		*allIt = getUserStatistics(usersIt->username); //get the data of each to the 'all' container
	}

	//sort the 'all' container to get the top 5
	std::sort(all.begin(), all.end(), 
		//sort function on each
		[](UserStatistics& first, UserStatistics& second) {
			return first.score > second.score; //big to small, by score
		});

	limit = std::min<int>((int)all.size(), GET_TOP);

	topFive.assign(all.begin(), all.begin() + limit);
	return topFive;
}
