#include "LoginManager.h"

LoginManager* LoginManager::instance = nullptr;

LoginManager& LoginManager::get() {
	if (LoginManager::instance == nullptr) {
		LoginManager::instance = new LoginManager();
	}

	return *LoginManager::instance;
}

void LoginManager::destroy() {
	delete LoginManager::instance;
	LoginManager::instance = nullptr;
}

LoginManager::LoginManager() : _db(SqliteDatabase::get()) {
	this->_loggedUsers.clear();
}

LoginManager::~LoginManager() {

	this->_loggedUsers.resize(0);
	this->_loggedUsers.clear();
}

bool LoginManager::signup(const std::string& username, const std::string& password, const std::string& email) {
	bool signedUp = true;

	signedUp = this->_db.addNewUser(username, password, email);

	return signedUp;
}

bool LoginManager::login(const std::string& username, const std::string& password) {
	bool loggedIn = true;

	loggedIn = this->_db.doesPasswordMatch(username, password);

	if (loggedIn) {
		insertToLoggedUsers(username);
	}

	return loggedIn;
}

void LoginManager::logout(const std::string& username) {
	int index = 0;
	LoggedUser usr(username);
	std::lock_guard<std::mutex>* lock = new std::lock_guard<std::mutex>(this->_loggedUsersLock); //lock for loop

	std::vector<LoggedUser>::iterator remove = this->_loggedUsers.end();

	std::vector<LoggedUser>::iterator it = this->_loggedUsers.begin();
	for (it = it; it != this->_loggedUsers.end(); ++it) {
		if (usr.username == it->username) {
			remove = it;
		}
	}
	
	delete lock;
	if (remove != this->_loggedUsers.end()) {
		deleteFromLoggedUser(remove);
	}
}

void LoginManager::insertToLoggedUsers(const std::string& user) {
	std::lock_guard<std::mutex> lock(this->_loggedUsersLock);
	this->_loggedUsers.emplace_back(user);
}

void LoginManager::deleteFromLoggedUser(const std::vector<LoggedUser>::iterator& user) {
	std::lock_guard<std::mutex> lock(this->_loggedUsersLock);
	this->_loggedUsers.erase(user);
}

bool LoginManager::isLoggedIn(const std::string& username) {
	std::lock_guard<std::mutex> lock(this->_loggedUsersLock);
	bool found = false;

	for (auto& elem : this->_loggedUsers) {
		if (elem.username == username) {
			found = true;
		}
	}

	return found;
}
