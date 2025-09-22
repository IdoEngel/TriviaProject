#include "GameManager.h"

GameManager* GameManager::_instance = nullptr;

GameManager& GameManager::get() {
	if (GameManager::_instance == nullptr) {
		GameManager::_instance = new GameManager();
	}

	return *GameManager::_instance;
}
void GameManager::destroy() {
	delete GameManager::_instance;
	GameManager::_instance = nullptr; 
}

GameManager::GameManager() {
	this->_db = &SqliteDatabase::get();
}

std::shared_ptr<Game> GameManager::createGame(const std::shared_ptr<Room> room) {
	//add the game data to the instance
	QuestionDbPass allQuestions;

	int gameID = this->_db->getLastGameID() + 1;

	//create the game
	std::shared_ptr<Game> gameCreated = std::make_shared<Game>(gameID);

	//add data - players and questions
	gameCreated->addPlayers(room);
	allQuestions = this->_db->getQuestions(room->getRoomData().numOfQuestions);

	gameCreated->addQuestions(allQuestions);

	//add empty game to the DB - update later 
	SqliteDatabase::get().createGameOnDB(gameCreated);

	//add the game to the vector
	std::lock_guard<std::mutex> lock(this->_gamesProtect);
	this->_games.push_back(gameCreated);

	return gameCreated;
}

void GameManager::deleteGame(const int gameID) {
	std::vector<std::shared_ptr<Game>>::iterator deleteIt = this->_games.end();

	std::lock_guard<std::mutex> lock(this->_gamesProtect);
	//for each search for the one with the right ID
	std::vector<std::shared_ptr<Game>>::iterator it;
	for (it = this->_games.begin(); it != this->_games.end(); ++it) {

		//if the game ID in the vector 
		if (it->get()->getGameID() == gameID) {
			deleteIt = it;
		}
	}

	if (deleteIt != this->_games.end()) { //if deleteIt still .end() - not found any - do nothing
		this->_games.erase(deleteIt);
	}
}

std::shared_ptr<Game> GameManager::findUsersGame(const std::string& username) const {
	std::shared_ptr<Game> gameFound;
	std::map <LoggedUser, GameData> currPlayers; //players in the game

	std::lock_guard<std::mutex> lock(this->_gamesProtect);
	for (const std::shared_ptr<Game> game : this->_games) {

		currPlayers = game->getPlayers();

		//if the player is found ( .find() return the iterator of the found object, end if not found )
		if (currPlayers.find(LoggedUser(username)) != currPlayers.end()) {
			gameFound = game; //create a copy of the room
		}

		currPlayers.clear();
	}

	return gameFound;
}
