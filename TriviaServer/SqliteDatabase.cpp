#include "SqliteDatabase.h"

using namespace DB_Names;

const std::string SqliteDatabase::_dbName = "DB_Trivia.sqlite";

SqliteDatabase* SqliteDatabase::instance = nullptr;


SqliteDatabase& SqliteDatabase::get() {
	if (SqliteDatabase::instance == nullptr) {
		SqliteDatabase::instance = new SqliteDatabase();
		SqliteDatabase::instance->open();
		SqliteDatabase::instance->insertQuestions();
	}

	return *SqliteDatabase::instance;
}

void SqliteDatabase::destroy() {
	SqliteDatabase::instance->close();
	delete SqliteDatabase::instance;
	SqliteDatabase::instance = nullptr;
}

void SqliteDatabase::doQuery(const std::string& query, callbackP callback, void* pass) {
	std::lock_guard<std::mutex> lock(this->_protect);

	int res = 0;
	char* errMsg = nullptr;
	bool isSuccessful = false;
	std::string errMsgCopy = "";

	res = sqlite3_exec(this->_handle, query.c_str(), callback, pass, &errMsg);

	if (res != SQLITE_OK) {

		errMsgCopy.assign(errMsg);
		if (errMsg) { //free memo
			sqlite3_free(errMsg);
		}

		sqlite3_exec(this->_handle, "ROLLBACK;", nullptr, nullptr, &errMsg); //delete changes in DB
		
		throw DB_Exception("Running query led to problems. SQLite error - " + errMsgCopy);
	}
	else {
		sqlite3_exec(this->_handle, "COMMIT;", nullptr, nullptr, &errMsg);  //commit changes in DB
	}
}

SqliteDatabase::SqliteDatabase() {
	this->_handle = nullptr;
	this->_openedSuccessfully = false;
}

SqliteDatabase::~SqliteDatabase() {
	this->close();
	this->_handle = nullptr;
}

bool SqliteDatabase::open() noexcept {
	bool isSuccessful = true;
	int res = 0;

	if (needToCreateDB(SqliteDatabase::_dbName)) { //create new file and tables

		res = sqlite3_open(SqliteDatabase::_dbName.c_str(), &(this->_handle));
		if (res != SQLITE_OK) {
			close();
			isSuccessful = false;
		}

		try {
			res = sqlite3_exec(this->_handle, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr); //allow foreign keys
			if (res != SQLITE_OK)
				throw DB_Exception("Cannot allow foreign keys on this DB");

			//create all the tables
			createUserTable();
			createQuestionTable();
			createStatisticsTable();
			createGameTable();
			createKeyTable();
		}
		catch (const DB_Exception& e) {
			close();
			Console::log(e.what(), Console::LogType::DB_Fail);
			isSuccessful = false;
		}
	}
	else { //open the file existing
		res = sqlite3_open(SqliteDatabase::_dbName.c_str(), &(this->_handle));
		if (res != SQLITE_OK) {
			close();
			isSuccessful = false;
		}

		res = sqlite3_exec(this->_handle, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr); //allow foreign keys
		if (res != SQLITE_OK) {
			Console::log("Cannot allow foreign keys on this DB", Console::LogType::DB_Fail);
			close();
		}
	}

	return isSuccessful;
}

bool SqliteDatabase::close() noexcept {
	int res = 0;
	res = sqlite3_close(this->_handle);
	this->_handle = nullptr;

	this->_openedSuccessfully = false;

	return res;
}

bool SqliteDatabase::doesUserExist(const std::string& username) {
	std::stringstream query;
	UserPass users;

	query << "BEGIN;\n";
	query << "SELECT * FROM " << UserTable::tName << "\n"
		"	WHERE " << UserTable::PK_Name << " = '" << username << "';\n";

	this->_container.setInsert(Entity::ToInsert::User);

	doQuery(query.str(), Entity::callback, &(this->_container));
	
	users = this->_container.getUsers();

	return users->size() != 0;
}

bool SqliteDatabase::doesPasswordMatch(const std::string& username, const std::string& password) {
	std::stringstream query;
	UserPass users;

	query << "BEGIN;\n";
	query << "SELECT * FROM " << UserTable::tName << "\n"
		"	WHERE " << UserTable::PK_Name << " = '" << username << "' \n"
		"	AND " << UserTable::passwordName << " = '" << password << "';\n";

	this->_container.setInsert(Entity::ToInsert::User);

	doQuery(query.str(), Entity::callback, &(this->_container));

	users = this->_container.getUsers();

	return users->size() != 0;
}

bool SqliteDatabase::addNewUser(const std::string& username, const std::string& password, const std::string& email) {
	std::stringstream query;
	bool inserted = true;

	query << "BEGIN;\n";
	query << "INSERT INTO " << UserTable::tName << "\n"
		"(" << UserTable::PK_Name << ", " << UserTable::passwordName << ", " << UserTable::emailName << ")\n"
		"VALUES\n"
		"( '" << username << "', '" << password << "', '" << email << "');\n";

	//insert into the statistics table empty data
	query << "INSERT INTO " << StatisticsTable::tName << "\n" <<
		"(" << StatisticsTable::userKey << ", " << StatisticsTable::correctAnswers << ", " << StatisticsTable::averageAnsTime << ")\n" <<
		"VALUES\n" <<
		"( '" << username << "', 0, 0);\n";

	try {
		doQuery(query.str());
	}
	catch (const DB_Exception& e) {
		Console::log(e.what(), Console::LogType::DB_Fail);
		inserted = false;
	}

	return inserted;
}

UserPass SqliteDatabase::getUsers() {
	UserPass users;
	std::stringstream query;

	query << "BEGIN;\n\n";
	query << "SELECT * FROM " << UserTable::tName << ";";

	this->_container.setInsert(Entity::ToInsert::User);
	
	doQuery(query.str(), Entity::callback, &(this->_container));

	users = this->_container.getUsers();

	return users;
}

QuestionDbPass SqliteDatabase::getQuestions(const int amount) {
	QuestionDbPass questions;
	std::stringstream query;

	query << "BEGIN;\n\n" <<
		"SELECT * FROM " << QuestionTable::tName << "\n" <<
		"INNER JOIN " << IncorrectAnsTable::tName << "\n" <<
		"	ON " << QuestionTable::tName << "." << QuestionTable::PK_Name << " = " <<
		IncorrectAnsTable::tName << "." << IncorrectAnsTable::questionKey << "\n" <<
		"LIMIT " << amount << ";";

	this->_container.setInsert(Entity::ToInsert::Question);

	doQuery(query.str(), Entity::callback, &(this->_container));

	questions = this->_container.getQuestions();

	return questions;
}

void SqliteDatabase::insertQuestions() {
	std::stringstream query;
	opentdbConnection connection;
	nlohmann::json data;
	QuestionDB insert;

	if (connection.connectToRemote()) {
		doQuery("DELETE FROM " + QuestionTable::tName); //delete all pervious questions

		data = connection.getData();

		//for every question
		for (auto& elem : data) {
			insert.question = elem[jsonFieldNames::question];
			insert.correct = elem[jsonFieldNames::correctAns];

			//insert the incorrect ans
			int i = 0;
			for (i = 0; i < NUM_OF_INCORRECT_ANSWERS; i++) {
				insert.incorrect[i] = elem[jsonFieldNames::incorrectAns][i];
			}

			//do the queries
			query.str("");

			//insert every question
			//insert into the question table
			query << "BEGIN;\n\n";
			query << "INSERT INTO " << QuestionTable::tName << "\n" <<
				"(" << QuestionTable::PK_Name << "," << QuestionTable::correctAns << ")\n"
				"VALUES ('" << insert.question << "','" << insert.correct << "');\n\n\n";

			//insert into  the incorrect answers table
			query << "INSERT INTO " << IncorrectAnsTable::tName << "\n" <<
				"(" << IncorrectAnsTable::questionKey << "," << IncorrectAnsTable::incorrect1 << "," <<
				IncorrectAnsTable::incorrect2 << "," << IncorrectAnsTable::incorrect3 << ")\n" <<
				//insert the values
				"VALUES ('" << insert.question << "','" << insert.incorrect[0] << "','" <<
				insert.incorrect[1] << "','" << insert.incorrect[2] << "');";

			doQuery(query.str());
 		}
	}

}

float SqliteDatabase::getPlayerAverageAnswerTime(const std::string& username) {
	float aveAnsTime = 0;
	std::stringstream query;
	StatisticsPass queryAns;

	query << "BEGIN;\n\n";
	query << "SELECT * FROM " << StatisticsTable::tName << "\n" <<
		"	WHERE " << StatisticsTable::userKey << " = '" << username << "';";

	this->_container.setInsert(Entity::ToInsert::Statistics);
	doQuery(query.str(), Entity::callback, &(this->_container));

	queryAns = this->_container.getStatistics();

	//loop for every statistic of the user (every game gets new statistics row
	std::list<Statistics>::iterator it = queryAns->begin();
	for (it = it; it != queryAns->end(); ++it) {
		aveAnsTime += it->averageAnsTime;
	}
	aveAnsTime /= queryAns->size() -1; //devide to create the average, -1 because the first draft row when user created

	return aveAnsTime;
}

int SqliteDatabase::getNumOfCorrectAnswers(const std::string& username) {
	int correctAns = 0;
	std::stringstream query;
	StatisticsPass queryAns;

	query << "BEGIN;\n\n";
	query << "SELECT * FROM " << StatisticsTable::tName << "\n" <<
		"WHERE " << StatisticsTable::userKey << " = '" << username << "'\n" <<
		";";

	this->_container.setInsert(Entity::ToInsert::Statistics);
	doQuery(query.str(), Entity::callback, &(this->_container));

	queryAns = this->_container.getStatistics();

	//sum the number of correct answers of all the game
	std::list<Statistics>::iterator it = queryAns->begin();
	for (it = it; it != queryAns->end(); ++it) {
		correctAns += it->numOfCorrectAns;
	}

	return correctAns;
}

int SqliteDatabase::getNumOfTotalAnswers(const std::string& username) {
	int totalAns = 0;
	std::stringstream query;
	GamePass queryAns;

	query << "BEGIN;\n\n";
	query << "SELECT * FROM \n" <<
		"	" << KeyTable::tName << " INNER JOIN " << GameTable::tName << "\n" <<
		"	ON " << KeyTable::tName << "." << KeyTable::gameKey << " = " << GameTable::tName << "." << GameTable::PK_Name << "\n" <<
		"	WHERE " << KeyTable::tName << "." << KeyTable::userKey << " = '" << username << "'\n" <<
		";";

	this->_container.setInsert(Entity::ToInsert::Game);
	doQuery(query.str(), Entity::callback, &(this->_container));

	queryAns = this->_container.getGames();

	//count the total number of questions in every game the user played
	std::list<GameDB>::const_iterator it = queryAns->begin();
	for (it = it; it != queryAns->end(); ++it) {
		totalAns += it->questionCount;
	}

	return totalAns;
}

int SqliteDatabase::getNumOfGamesOfPlayers(const std::string& username) {
	int gamesCount = 0;
	std::stringstream query;
	GamePass queryAns;

	query << "BEGIN;\n\n";
	query << "SELECT * FROM \n" <<
		"	" << KeyTable::tName << " INNER JOIN " << GameTable::tName << "\n" <<
		"	ON " << KeyTable::tName << "." << KeyTable::gameKey << " = " << GameTable::tName << "." << GameTable::PK_Name << "\n" <<
		"	WHERE " << KeyTable::tName << "." << KeyTable::userKey << " = '" << username << "'\n" <<
		";";

	this->_container.setInsert(Entity::ToInsert::Game);
	doQuery(query.str(), Entity::callback, &(this->_container));

	queryAns = this->_container.getGames();
	gamesCount = queryAns->size();

	return gamesCount;
}

int SqliteDatabase::getPlayerScore(const std::string& username) {
	float total = 0;
	int numOfCorrectAns = 0;

	numOfCorrectAns = getNumOfCorrectAnswers(username);
	if (numOfCorrectAns != 0)/*Prevent zero-division-error*/ {
		total = (getNumOfTotalAnswers(username) / numOfCorrectAns);
		total += getPlayerAverageAnswerTime(username);
	}
	
	return static_cast<int>(std::round(total));
}

void SqliteDatabase::submitGameStatistics(const std::shared_ptr<Game> data) {
	std::stringstream query;
	std::stringstream keyTableValues;
	std::stringstream statisticsTableValues;

	std::string keyTableStr;
	std::string statisticsTableStr;

	std::map<LoggedUser, GameData> players;

	//create a game column
	query << "BEGIN;\n\n";
	query << "UPDATE " << GameTable::tName << "\n";
	query << "SET " << GameTable::numOfQuestions << " = " << data->getNumberOfQuestions();
	query << "	WHERE " << GameTable::PK_Name << " = " << data->getGameID() << ";\n\n\n";

	//insert the data buffers to add to the query
	players = data->getPlayers();
	for (auto [username, metadata] : players) {
		//insert the key data
		keyTableValues << "( '" << username.username << "', " << data->getGameID() << " ),";

		//inserting the statistics table
		statisticsTableValues << "( '" << username.username << "', " << metadata.correctAnsCount <<
			", " << metadata.aveAnsTime << " ),";
	}

	//convert into a string and remove the last character (a comma) - for key table
	keyTableStr = keyTableValues.str();
	keyTableStr.pop_back();

	query << "INSERT INTO " << KeyTable::tName << "\n";
	query << "(" << KeyTable::userKey << ", " << KeyTable::gameKey << ")\n";
	query << "VALUES\n";
	query << "	" << keyTableStr << ";\n\n\n";

	//convert into a string and remove the last character (a comma) - for statistics table
	statisticsTableStr = statisticsTableValues.str();
	statisticsTableStr.pop_back();

	query << "INSERT INTO " << StatisticsTable::tName << "\n";
	query << "(" << StatisticsTable::userKey << ", " << StatisticsTable::correctAnswers << ", " << 
		StatisticsTable::averageAnsTime << ")\n";
	query << "VALUES\n";
	query << "	" << statisticsTableStr << ";\n\n\n";

	doQuery(query.str());
}

void SqliteDatabase::createGameOnDB(const std::shared_ptr<Game> data) {
	std::stringstream query;

	//create a game column
	query << "BEGIN;\n\n";
	query << "INSERT INTO " << GameTable::tName << "\n";
	query << "(" << GameTable::PK_Name << ", " << GameTable::numOfQuestions << ")\n";
	query << "VALUES\n";
	query << "	( " << data->getGameID() << ", " << 0 << " );\n";

	doQuery(query.str());
}

int SqliteDatabase::getLastGameID() {
	int* ret = nullptr;
	int retNoPtr = 0; //default value - wont change if no value in the DB

	sqlite3_exec(this->_handle, (std::string("SELECT seq FROM sqlite_sequence WHERE name = '") + GameTable::tName + "';").c_str(),
		[](void* pass, int argc, char** argv, char** azColName) -> int { /*Lambda Function*/
			if (argc == 1) { //if data exists
				*reinterpret_cast<int**>(pass) = new int(std::stoi(argv[0]));
			}
			return 0;
		}, &ret, nullptr);

	if (ret != nullptr) { //if nullptr - no data yet - start from 1
		retNoPtr = *ret; //copy to non-leaking type
	}
	delete ret;

	return retNoPtr;
}

bool SqliteDatabase::needToCreateDB(const std::string& dbName) const noexcept {
	return _access(dbName.c_str(), 0) != 0;
}


void SqliteDatabase::createUserTable() {
	int res = 0;
	char* errMsg = nullptr;
	std::string query = "";

	query = "CREATE TABLE IF NOT EXISTS " + UserTable::tName + " (\n"
		"	" + UserTable::PK_Name + " TEXT PRIMARY KEY,\n"
		"	" + UserTable::emailName + " TEXT NOT NULL,\n"
		"	" + UserTable::passwordName + " TEXT NOT NULL\n"
		");\n";


	res = sqlite3_exec(this->_handle, query.c_str(), nullptr, nullptr, &errMsg);

	if (res != SQLITE_OK) {
		throw DB_Exception("Cannot create User table. SQLite error - " + std::string(errMsg));
	}
}


void SqliteDatabase::createQuestionTable() {
	int resQuestionTable = 0;
	int resIncorrectAnsTable = 0;
	char* errMsg = nullptr;
	std::stringstream query;

	sqlite3_exec(this->_handle, "BEGIN;", nullptr, nullptr, &errMsg);
	if (errMsg) { //free memo
		sqlite3_free(errMsg);
	}

	//create the question table
	query << "CREATE TABLE IF NOT EXISTS " << QuestionTable::tName << " (\n" <<
		"	" << QuestionTable::PK_Name << " TEXT PRIMARY KEY,\n" <<
		"	" << QuestionTable::correctAns << " TEXT NOT NULL\n" <<
		");";

	resQuestionTable = sqlite3_exec(this->_handle, query.str().c_str(), nullptr, nullptr, &errMsg);
	if (errMsg) { //free memo
		sqlite3_free(errMsg);
	}

	query.str("");
	std::cout << query.str();
	//create the incorrect answers table for each question
	query << "CREATE TABLE IF NOT EXISTS " << IncorrectAnsTable::tName << " (\n" <<
		"	" << IncorrectAnsTable::questionKey << " TEXT NOT NULL,\n" <<
		"	" << IncorrectAnsTable::incorrect1 << " TEXT NOT NULL,\n" <<
		"	" << IncorrectAnsTable::incorrect2 << " TEXT NOT NULL,\n" <<
		"	" << IncorrectAnsTable::incorrect3 << " TEXT NOT NULL,\n\n" <<
		//foreign key
		"	FOREIGN KEY (" << IncorrectAnsTable::questionKey << ") REFERENCES " << QuestionTable::tName << "(" <<
			QuestionTable::PK_Name << ") ON DELETE CASCADE\n" << //delete row if foreign key deleted
		");";

	resIncorrectAnsTable = sqlite3_exec(this->_handle, query.str().c_str(), nullptr, nullptr, &errMsg);
	if (errMsg) { //free memo
		sqlite3_free(errMsg);
	}

	if (resIncorrectAnsTable != SQLITE_OK || resQuestionTable != SQLITE_OK) {
		sqlite3_exec(this->_handle, "ROLLBACK;", nullptr, nullptr, &errMsg);
		if (errMsg) { //free memo
			sqlite3_free(errMsg);
		}
		
		throw DB_Exception("Cannot create the Question or the IncorrectAns tables");
	}
	else {
		sqlite3_exec(this->_handle, "COMMIT;", nullptr, nullptr, &errMsg);
		if (errMsg) { //free memo
			sqlite3_free(errMsg);
		}
	}
}

void SqliteDatabase::createStatisticsTable() {
	int res = 0;
	char* errMsg = nullptr;
	std::stringstream query;
	std::string errCopy;

	query << "CREATE TABLE IF NOT EXISTS " << StatisticsTable::tName << " (\n" <<
		"	" << StatisticsTable::userKey << " TEXT NOT NULL,\n" <<
		"	" << StatisticsTable::correctAnswers << " INTERER NOT NULL,\n" <<
		"	" << StatisticsTable::averageAnsTime << " REAL NOT NULL,\n" << //floating point number
		"	FOREIGN KEY (" << StatisticsTable::userKey << ") REFERENCES " << UserTable::tName << "(" <<
		UserTable::PK_Name << ") ON DELETE CASCADE\n" << //delete row if foreign key deleted
		");";
	
	res = sqlite3_exec(this->_handle, query.str().c_str(), nullptr, nullptr, &errMsg);
	if (errMsg) { //free memo
		errCopy = errMsg;
		sqlite3_free(errMsg);
	}

	if (res != SQLITE_OK) {
		throw DB_Exception("Cannot create Statistics table. SQLite error - " + errCopy);
	}

}

void SqliteDatabase::createGameTable() {
	int res = 0;
	char* errMsg = nullptr;
	std::stringstream query;
	std::string errCopy;

	query << "CREATE TABLE IF NOT EXISTS " << GameTable::tName << "(\n" <<
		"	" << GameTable::PK_Name << " INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,\n" <<
		"	" << GameTable::numOfQuestions << " INTEGER NOT NULL\n" <<
		");";

	res = sqlite3_exec(this->_handle, query.str().c_str(), nullptr, nullptr, &errMsg);
	if (errMsg) { //free memo
		errCopy = errMsg;
		sqlite3_free(errMsg);
	}

	if (res != SQLITE_OK) {
		throw DB_Exception("Cannot create Game table. SQLite error - " + errCopy);
	}
}

void SqliteDatabase::createKeyTable() {
	int res = 0;
	char* errMsg = nullptr;
	std::stringstream query;
	std::string errCopy;

	query << "CREATE TABLE IF NOT EXISTS " << KeyTable::tName << "(\n" <<
		"	" << KeyTable::userKey << " TEXT NOT NULL,\n" <<
		"	" << KeyTable::gameKey << " INTEGER NOT NULL,\n\n" <<
		//insert foreign keys
		"	FOREIGN KEY (" << KeyTable::userKey << ") REFERENCES " << UserTable::tName << "(" <<
		UserTable::PK_Name << ") ON DELETE CASCADE,\n"
		"	FOREIGN KEY (" << KeyTable::gameKey << ") REFERENCES " << GameTable::tName << "(" <<
		GameTable::PK_Name << ") ON DELETE CASCADE\n" <<
		");";


	res = sqlite3_exec(this->_handle, query.str().c_str(), nullptr, nullptr, &errMsg);
	if (errMsg) { //free memo
		errCopy = errMsg;
		sqlite3_free(errMsg);
	}

	if (res != SQLITE_OK) {
		throw DB_Exception("Cannot create Key table. SQLite error - " + errCopy);
	}
}
