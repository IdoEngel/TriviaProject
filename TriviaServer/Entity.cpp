#include "Entity.h"

Entity::~Entity() {
	this->_gameCash.clear();
	this->_keyCash.clear();
	this->_questionsCash.clear();
	this->_statisticsCash.clear();
	this->_usersCash.clear();
}

int Entity::callback(void* data, int argc, char** argv, char** azColName) {
	Entity* handle = nullptr;
	std::string currFieldName = "";

	//user fields
	std::string username = "";
	std::string password = "";
	std::string email = "";

	//question fields
	std::string question;
	std::string correctAns;
	std::string incoQuestionOne;
	std::string incoQuestionTwo;
	std::string incoQuestionThree;

	//statistics fields
	//username is been used here too
	int numOfCorrectAns = 0;
	float aveAnsTime = 0;

	//game fields
	int gameID = 0;
	int numOfQuestionsInGame = 0;

	handle = static_cast<Entity*>(data);

	int i = 0;
	for (i = 0; i < argc; i++) { //for each field that returned
		currFieldName = std::string(azColName[i]);

		switch (handle->_elemToInsert) //select the right fields to get the data into 
		{
		case Entity::ToInsert::User:
			//insert data from DB ans
			if (currFieldName == DB_Names::UserTable::PK_Name)
				username = std::string(argv[i]);
			else if (currFieldName == DB_Names::UserTable::emailName)
				email = std::string(argv[i]);
			else if (currFieldName == DB_Names::UserTable::passwordName)
				password = std::string(argv[i]);
			break;

		case Entity::ToInsert::Question:
			if (currFieldName == DB_Names::QuestionTable::PK_Name)
				question = std::string(argv[i]);
			if (currFieldName == DB_Names::QuestionTable::correctAns)
				correctAns = std::string(argv[i]);
			if (currFieldName == DB_Names::IncorrectAnsTable::incorrect1)
				incoQuestionOne = std::string(argv[i]);
			if (currFieldName == DB_Names::IncorrectAnsTable::incorrect2)
				incoQuestionTwo = std::string(argv[i]);
			if (currFieldName == DB_Names::IncorrectAnsTable::incorrect3)
				incoQuestionThree = std::string(argv[i]);
			break;

		case Entity::ToInsert::Statistics:
			if (currFieldName == DB_Names::StatisticsTable::userKey)
				username = std::string(argv[i]);
			if (currFieldName == DB_Names::StatisticsTable::correctAnswers)
				numOfCorrectAns = std::stoi(argv[i]);
			if (currFieldName == DB_Names::StatisticsTable::averageAnsTime)
				aveAnsTime = std::stof(argv[i]);
			break;

		case Entity::ToInsert::Game:
			if (currFieldName == DB_Names::GameTable::PK_Name)
				gameID = std::stoi(argv[i]);
			if (currFieldName == DB_Names::GameTable::numOfQuestions)
				numOfQuestionsInGame = std::stoi(argv[i]);
			break;

		case Entity::ToInsert::Key:
			//collect user data
			if (currFieldName == DB_Names::UserTable::PK_Name)
				username = std::string(argv[i]);
			if (currFieldName == DB_Names::UserTable::passwordName)
				password = std::string(argv[i]);
			if (currFieldName == DB_Names::UserTable::emailName)
				email = std::string(argv[i]);

			//collect game data
			if (currFieldName == DB_Names::GameTable::PK_Name)
				gameID = std::stoi(argv[i]);
			if (currFieldName == DB_Names::GameTable::numOfQuestions)
				numOfQuestionsInGame = std::stoi(argv[i]);
			break;
		}
	}

	switch (handle->_elemToInsert) //insert into the cashes
	{
	case Entity::ToInsert::User:
		handle->_usersCash.emplace_back(username, password, email);
		break;
	case Entity::ToInsert::Question:
		handle->_questionsCash.emplace_back(question, correctAns, incoQuestionOne, incoQuestionTwo, incoQuestionThree);
		break;
	case Entity::ToInsert::Statistics:
		handle->_statisticsCash.emplace_back(username, numOfCorrectAns, aveAnsTime);
		break;
	case Entity::ToInsert::Game:
		handle->_gameCash.emplace_back(gameID, numOfQuestionsInGame);
		break;
	case Entity::ToInsert::Key:
		handle->_keyCash.emplace_back(username, password, email, gameID, numOfQuestionsInGame);
		break;
	}

	return 0;
}

void Entity::setInsert(const ToInsert& insert) {
	this->_elemToInsert = insert;
}

Entity::ToInsert Entity::getInsert() const {
	return this->_elemToInsert;
}

UserPass Entity::getUsers() {
	UserPass retVal = std::make_shared<std::list<User>>();

	for (const User& user : this->_usersCash) {
		retVal->push_back(user);
	}

	this->_usersCash.clear();
	return retVal;
}

QuestionDbPass Entity::getQuestions() {
	QuestionDbPass retVal = std::make_shared<std::list<QuestionDB>>();

	for (const QuestionDB& que : this->_questionsCash) {
		retVal->push_back(que);
	}

	this->_questionsCash.clear();
	return retVal;
}

StatisticsPass Entity::getStatistics() {
	StatisticsPass retVal = std::make_shared<std::list<Statistics>>();

	for (const Statistics& elem : this->_statisticsCash) {
		retVal->push_back(elem);
	}

	this->_statisticsCash.clear();
	return retVal;
}

GamePass Entity::getGames() {
	GamePass retVal = std::make_shared<std::list<GameDB>>();

	for (const GameDB& gm : this->_gameCash) {
		retVal->push_back(gm);
	}

	this->_gameCash.clear();
	return retVal;
}

KeyPass Entity::getKeys() {
	KeyPass retVal = std::make_shared<std::list<Key>>();

	for (const Key& k : this->_keyCash) {
		retVal->push_back(k);
	}

	this->_keyCash.clear();
	return retVal;
}
