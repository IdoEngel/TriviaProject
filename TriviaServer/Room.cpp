#include "Room.h"

std::mutex Room::_usersProtect;

Room::Room(const RoomData& metadata, const std::string& admin, std::vector<LoggedUser>* users) {
	this->_metadata = metadata;
	this->_admin = admin;

	//copy the vector..
	if (users != nullptr) {
		auto insertIt = std::back_insert_iterator<std::vector<LoggedUser>>(this->_users);
		std::copy(users->begin(), users->end(), insertIt);
	}

	fixNumOfQuestions();
}

Room::Room(const unsigned int id, const std::string& name,
	const unsigned int maxPlayers, const unsigned int numOfQuestions,
	const unsigned int timePerQuestion, const bool isActive,
	const std::string& admin,
	std::vector<LoggedUser>* users) {
	
	this->_metadata = RoomData(id, name, maxPlayers, numOfQuestions, timePerQuestion, isActive);
	this->_admin = admin;

	//for the vector..
	if (users != nullptr) {
		auto insertIt = std::back_insert_iterator<std::vector<LoggedUser>>(this->_users);
		std::copy(users->begin(), users->end(), insertIt);
	}

	fixNumOfQuestions();
}

Room::~Room() {
	this->_users.resize(0);
	this->_users.clear();
}

void Room::addUser(const LoggedUser& user) {
	std::lock_guard<std::mutex> lock(Room::_usersProtect);
	this->_users.push_back(user);
}

bool Room::doesUserInRoom(const LoggedUser& user) {
	std::lock_guard<std::mutex> lock(Room::_usersProtect);

	//std::find return .end() if element NOT found
	return std::find(this->_users.begin(), this->_users.end(), user) != this->_users.end();
}

bool Room::doesUserAdminOfTheRoom(const std::string& user) {
	return user == this->_admin;
}

std::string Room::getAdmin() const {
	return this->_admin;
}

bool Room::isRoomFull() {
	int numOfUsers = 0;
	int capacity = 0;

	std::lock_guard<std::mutex> lockMeta(this->_metadataProtect);
	capacity = this->_metadata.maxPlayers;

	std::lock_guard<std::mutex> lockUsers(this->_usersProtect);
	numOfUsers = this->_users.size();

	//'true' if the num of users is currently in the vector is more or equel to the capacity
	return numOfUsers >= capacity;
}

void Room::removeUser(const LoggedUser& user) {
	std::lock_guard<std::mutex> lock(Room::_usersProtect);
	std::vector<LoggedUser>::iterator removeIt = this->_users.end();

	//find the element iterator
	std::vector<LoggedUser>::iterator it;
	for (it = this->_users.begin(); it != this->_users.end(); ++it) {
		if (it->username == user.username) {
			removeIt = it;
		}
	}

	if (removeIt != this->_users.end()) {
		this->_users.erase(removeIt);
	}
}

const std::vector<LoggedUser> const Room::getAllUsers() const {
	std::lock_guard<std::mutex> lock(Room::_usersProtect);
	return this->_users;
}

int Room::usersLen(const std::vector<LoggedUser>& v) {
	std::lock_guard<std::mutex> lock(Room::_usersProtect);

	return v.size();
}

LoggedUser Room::getUserByIndex(const std::vector<LoggedUser>& v, const int index) {
	std::lock_guard<std::mutex> lock(Room::_usersProtect);

	LoggedUser usr;

	if (index < v.size()) {
		usr = v[index];
	}

	return usr;
}

RoomData Room::getRoomData() {
	std::lock_guard<std::mutex> lock(this->_metadataProtect);

	return this->_metadata;
}

void Room::startGame() {
	std::lock_guard<std::mutex> lock(this->_metadataProtect);

	this->_metadata.isActive = true;
}

void Room::stopGame() {
	std::lock_guard<std::mutex> lock(this->_metadataProtect);

	this->_metadata.isActive = false;
}


bool Room::isActive() const {
	std::lock_guard<std::mutex> lock(this->_metadataProtect);
	
	return this->_metadata.isActive;
}

void Room::fixNumOfQuestions() {

	std::lock_guard<std::mutex> lock(this->_metadataProtect);

	//if more the max questions or less then 1
	if (this->_metadata.numOfQuestions > NUM_OF_QUESTIONS_IN_DB ||
		this->_metadata.numOfQuestions < 1) {

		this->_metadata.numOfQuestions = NUM_OF_QUESTIONS_IN_DB;
	}
}
