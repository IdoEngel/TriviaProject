#pragma once
#include <algorithm>
#include <map>
#include <mutex>
#include <memory>
#include "RequestResponseStructs.h"
#include "Room.h"
#include "NotifyRoomChanges.h"

class RoomManager
{
public:
	//singleton design
	RoomManager& operator=(const RoomManager&) = delete;
	RoomManager(const RoomManager&) = delete;

	/**
	* @brief gets the single instance of the class
	*/
	static RoomManager& get();

	/**
	* @brief deletes the single instance of the class
	*/
	static void destroy();

	/**
	* @brief creates a new room 
	* @brief adds the first user (admin) to the new room
	* @param user the user created the room
	* @para, admin the admin of the room
	* @param metadata the data of the room
	*/
	void createRoom(const LoggedUser& user, const std::string& admin, const RoomData& metadata);

	/**
	* @brief removes a room from the map
	* @param id the id of the room in the map
	*/
	void deleteRoom(const unsigned int id);

	/**
	* @brief gets the biggets ID stored in the vector of rooms
	* @brief ADD ONE to get an ID that not been used
	* @returns the curr biggest ID currently in the server
	*/
	unsigned int getLargestID();

	/**
	* @brief gets the room state (active/wait to start)
	* @param id the id of the room
	* @returns 'true' if game is active, 'false' if wait to start
	*/
	bool getRoomState(const unsigned int id) const;

	/**
	* @brief gets a vector of the rooms saved in the class
	* @returns a vector of rooms
	*/
	std::vector<std::shared_ptr<Room>> getRooms() const;

	/**
	* @brief gets a room by its ID
	* @param id the id of the room to get
	* @returns shared_ptr of the room
	*/
	std::shared_ptr<Room> getRoom(const unsigned int id) const;

	/**
	* @brief checks whether the room exists
	* @param id the ID of the room to check
	* @returns 'true' if the room exists
	*/
	bool isRoomExists(const unsigned int id) const;

	/**
	* @brief check if user in any room, and if it is, delete the user from the room
	* @brief if the user is the room manager, tell all the members that the room closed
	* @param username the username to release
	*/
	void releaseUser(const std::string& username);

	/**
	* @brief delete rooms with no users in
	*/
	void roomCleanup();

private:
	//for singleton
	RoomManager() = default;
	~RoomManager();
	static RoomManager* _instance;

	std::map<unsigned int, std::shared_ptr<Room>> _rooms;
	mutable std::mutex _roomsProtect;
};

