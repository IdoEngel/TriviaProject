#include "Game.h"
#include "SqliteDatabase.h"

Game::Game(const int gameID) {
	this->_gameID = gameID;
}

Game::Game(const Game& other) {
	this->_gameID = other._gameID;
	this->_numberOfQuestions = other._numberOfQuestions;

	this->_players.insert(other._players.begin(), other._players.end());

	//insert back iterator - insert 
	auto back = std::back_insert_iterator<std::vector<std::shared_ptr<Question>>>(this->_questions);
	std::copy(other._questions.begin(), other._questions.end(), back);
}

void Game::removePlayer(const LoggedUser& username) {
	std::lock_guard<std::mutex> lock(this->_playersProtect);
	this->_players.erase(username);
}

std::optional<std::shared_ptr<Question>> Game::getQuestionForUser(const LoggedUser& username) {
	std::optional<std::shared_ptr<Question>> nextQuestion = {}; //by default, the next question does not exist

	std::lock_guard<std::mutex> lockPlayers(this->_playersProtect); //lock the players
	std::map<LoggedUser, GameData>::iterator userIt = this->_players.find(username);

	try {
		std::lock_guard<std::mutex> lockQuestions(this->_questionsProtect); //lock the questions

		nextQuestion = this->_questions.at(userIt->second.questionNumber); // .at throws std::out_of_range

		userIt->second.currQuestion = nextQuestion.value()->getQuestion();
	}
	catch (const std::out_of_range&) {
		//if exception occurred - got the and of the list of questions, therefor optional will be empty
	}

	return nextQuestion;
}

void Game::submitAnswer(const LoggedUser& user, const int ansID, const int timeTook) {
	std::map<LoggedUser, GameData>::iterator userIt = this->_players.find(user);

	std::shared_ptr<Question> currQuestion = this->_questions.at(userIt->second.questionNumber);

	userIt->second.questionNumber++;

	//calc the new average
	if (userIt->second.aveAnsTime == 0) {
		userIt->second.aveAnsTime = timeTook;
	}
	else {
		userIt->second.aveAnsTime = (userIt->second.aveAnsTime * userIt->second.questionNumber + timeTook) / (userIt->second.questionNumber + 1);
	}


	//if the correct answer is the one user chose
	if (currQuestion->getCorrectAnsID() == ansID) {
		userIt->second.correctAnsCount++;
	}
	else {
		userIt->second.wrongAnsCount++;
	}
}

void Game::addQuestions(QuestionDbPass questions) {
	std::shared_ptr<Question> questionToInsert;

	std::lock_guard<std::mutex> lock(this->_questionsProtect);

	for (const QuestionDB& q : *questions.get()) {
		questionToInsert = std::make_shared<Question>(); //create new instance every time

		questionToInsert->insert(q);
		this->_questions.push_back(std::move(questionToInsert)); //replace the current version
	}

	//update the number of questions in the game
	this->_numberOfQuestions += questions->size();
}

bool Game::checkForEndGame(IDatabase* db) {
	bool allPlayersDone = true;

	std::lock_guard<std::mutex> lock(this->_playersProtect);
	for (auto [username, metadata] : this->_players) {

		//if the user didnot done answering all the questions
		if (metadata.questionNumber != this->_numberOfQuestions) {
			allPlayersDone = false;
		}
	}

	if (allPlayersDone) { //if all players done playing, update DB
		std::shared_ptr<Game> copyGame = std::make_shared<Game>(*this);
		dynamic_cast<SqliteDatabase*>(db)->submitGameStatistics(copyGame);
	}

	return allPlayersDone;
}

int Game::getGameID() const {
	return this->_gameID;
}

unsigned int Game::getNumberOfQuestions() const {
	return this->_numberOfQuestions;
}

unsigned int Game::getNumberOfMembers() const {
	std::lock_guard<std::mutex> lock(this->_playersProtect);

	return this->_players.size();
}

std::map<LoggedUser, GameData> Game::getPlayers() const {
	std::map<LoggedUser, GameData> ret;

	std::lock_guard<std::mutex> lock(this->_playersProtect);
	ret.insert(this->_players.begin(), this->_players.end()); //copy  the map to a new one

	return ret;
}

void Game::addPlayer(const LoggedUser& username) {
	std::lock_guard<std::mutex> lock(this->_playersProtect);
	this->_players.emplace(username, GameData());
}

void Game::addPlayers(const std::shared_ptr<Room> room) {
	const std::vector<LoggedUser> users = room->getAllUsers();
	int len = Room::usersLen(users);

	int i = 0;
	//for every player, use the Room interface for mutex safty
	for (i = 0; i < len; i++) {
		LoggedUser usr = Room::getUserByIndex(users, i);

		std::lock_guard<std::mutex> lock(this->_playersProtect);
		this->_players.emplace(usr, GameData());
	}
}

