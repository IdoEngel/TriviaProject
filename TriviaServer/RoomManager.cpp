#include "RoomManager.h"

RoomManager* RoomManager::_instance = nullptr;

RoomManager& RoomManager::get() {
	if (RoomManager::_instance == nullptr) {
		RoomManager::_instance = new RoomManager();
	}

	return *RoomManager::_instance;
}

void RoomManager::destroy() {
	delete RoomManager::_instance;
	RoomManager::_instance = nullptr;
}

RoomManager::~RoomManager() {
	this->_rooms.clear();
}

void RoomManager::createRoom(const LoggedUser& user, const std::string& admin, const RoomData& metadata) {
	std::shared_ptr<Room> rm = std::make_shared<Room>(metadata, admin);
	rm->addUser(user);
	unsigned int id = metadata.id;

	std::lock_guard<std::mutex> lock(this->_roomsProtect);
	this->_rooms.emplace(id, rm);
}

void RoomManager::deleteRoom(const unsigned int id) {
	std::lock_guard<std::mutex> lock(this->_roomsProtect);
	this->_rooms.erase(id);
}

unsigned int RoomManager::getLargestID() {
	unsigned int currMax = 1;

	std::lock_guard<std::mutex> lock(this->_roomsProtect);

	//for every pair
	std::map<unsigned int, std::shared_ptr<Room>>::iterator it = this->_rooms.begin();
	for (it = it; it != this->_rooms.end(); ++it) {
		//if the curr saved biggest is smaller then the ID of 'it'
		if (it->first > currMax) {
			currMax = it->first; //replace the saved biggest
		}
	}

	return currMax;
}

bool RoomManager::getRoomState(const unsigned int id) const {
	std::lock_guard<std::mutex> lock(this->_roomsProtect);

	return this->_rooms.find(id)->second.get()->getRoomData().isActive;
}

std::vector<std::shared_ptr<Room>> RoomManager::getRooms() const {
	std::lock_guard<std::mutex> lock(this->_roomsProtect);

	std::vector<std::shared_ptr<Room>> rm;
	rm.resize(this->_rooms.size());

	//transform the rooms in the map to rooms that will be stored in the vector
	std::transform(this->_rooms.begin(), this->_rooms.end(), rm.begin(),
		//this func will run on each element in the _rooms obj
		[](const std::pair<const unsigned int, std::shared_ptr<Room>>& it) -> std::shared_ptr<Room> {
			return it.second;
		});

	return rm;
}

std::shared_ptr<Room> RoomManager::getRoom(const unsigned int id) const {
	std::lock_guard<std::mutex> lock(this->_roomsProtect);

	return this->_rooms.find(id)->second;
}

bool RoomManager::isRoomExists(const unsigned int id) const {
	std::lock_guard<std::mutex> lock(this->_roomsProtect);

	return this->_rooms.find(id) != this->_rooms.end();
}

void RoomManager::releaseUser(const std::string& username) {
	std::shared_ptr<Room> usersRoom;
	usersRoom = nullptr;

	//find if the user is in a room
	std::lock_guard<std::mutex>* lock = new std::lock_guard<std::mutex>(this->_roomsProtect);

	std::map<unsigned int, std::shared_ptr<Room>>::iterator it = this->_rooms.begin();
	for (it = it; it != this->_rooms.end(); ++it) {
		if (it->second->doesUserInRoom(username)) {
			usersRoom = it->second;
		}
	}
	delete lock;

	if (usersRoom.get() != nullptr) { //if a room found
		NotifyRoomChanges triggers(usersRoom);

		//if the user is an admin
		if (usersRoom->doesUserAdminOfTheRoom(username)) {
			triggers.triggerCloseRoom();

			//delete the room
			this->deleteRoom(usersRoom->getRoomData().id);
		}
		else {
			triggers.triggerAdminLeftMember(username);

			//remove the user from the room
			this->getRoom(usersRoom->getRoomData().id)->removeUser(LoggedUser{ username });
		}
	}
}

void RoomManager::roomCleanup() {
	std::vector<int> idsToDelete;

	std::lock_guard<std::mutex> lock(this->_roomsProtect);

	//check all rooms, if there is a empty one
	for (auto [id, room] : this->_rooms) {
		//0 users = empty
		if (room->getAllUsers().size() == 0) {
			idsToDelete.push_back(id);
		}
	}

	//delete all the rooms that in the vector
	for (int id : idsToDelete) {
		this->_rooms.erase(id);
	}
}
