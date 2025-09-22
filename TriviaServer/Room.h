#pragma once
#include <mutex>
#include <algorithm>
#include <vector>
#include <iterator>
#include "RequestResponseStructs.h"

class Room
{
public:
	/**
	* @brief default c'tor for STL containers
	*/
	Room() = default;

	/**
	* @brief c'tor with RoomData struct as a param
	* @param metadata RoomData struct
	* @param admin the admin of the room
	* @param users the users (optional, default : nullptr) to insert to the room automatically
	*/
	Room(const RoomData& metadata, const std::string& admin, std::vector<LoggedUser>* users = nullptr);

	/**
	* @brief c'tor that creates the RoomData struct inside it
	* @param id for metadata (RoomData)
	* @param name for metadata (RoomData)
	* @param maxPlayers for metadata (RoomData)
	* @param numOfQuestions for metadata (RoomData)
	* @param timePerQuestion for metadata (RoomData)
	* @param isActive for metadata (RoomData)
	* @param admin the admin of the room
	* @param users (optional, default : nullptr) to insert to the room automatically
	*/
	Room(const unsigned int id, const std::string& name,
		const unsigned int maxPlayers, const unsigned int numOfQuestions,
		const unsigned int timePerQuestion, const bool isActive,
		const std::string& admin,
		std::vector<LoggedUser>* users = nullptr);

	//d'tor
	~Room();

	/**
	* @brief adds one user to the vector
	* @param user the user to add
	*/
	void addUser(const LoggedUser& user);

	/**
	* @brief checks if user is in the room
	* @param user the user to check
	* @return 'true' if the user is in the room
	*/
	bool doesUserInRoom(const LoggedUser& user);

	/**
	* @brief checks if the username provided is the username that is the admin of the room
	* @para user the user to check if the admin
	* @returns 'true' if the user is the admin
	*/
	bool doesUserAdminOfTheRoom(const std::string& user);

	/**
	* @brief gets the admin of the room
	* @returns the username of the admin of the room
	*/
	std::string getAdmin() const;

	/**
	* @brief checks if the room has more place to join more people 
	* @returns 'true' if the room is full (no more people can join)
	*/
	bool isRoomFull();

	/**
	* @brief removes the user provided from the vector
	* @param user the user to remove
	*/
	void removeUser(const LoggedUser& user);

	/**
	* @brief gets the vector (as const) of users in the room
	* @returns const reference of the vector inside the class
	*/
	const std::vector<LoggedUser> const getAllUsers() const;

	/**
	* @brief gets the len of the vector of the vector, outside of the class
	* @param v the vector to return its len
	*/
	static int usersLen(const std::vector<LoggedUser>& v);

	/**
	* @brief gets a single value from the vector, by index
	* @param v the vector to search in
	* @param index the index to get its value
	*/
	static LoggedUser getUserByIndex(const std::vector<LoggedUser>& v, const int index);

	/**
	* @brief gets the room data (metadata/RoomData struct)
	* @returns RoomData struct
	*/
	RoomData getRoomData();

	/**
	* @brief change the state of the game in the room to 'active' (true)
	*/
	void startGame();
	/**
	* @brief change the state of the game on the room to 'not active' (false)
	*/
	void stopGame();

	/**
	* @brief checks if the room is active
	* @return 'true' if the room is active
	*/
	bool isActive() const;

private:

	/**
	* @brief number of questions cannot be larger then the number of questions in the DB
	* @brief use constant RequestResponseStructs.h to fix the number of questions if too large
	*/
	void fixNumOfQuestions();
	RoomData _metadata;
	std::vector<LoggedUser> _users;
	std::string _admin;

	mutable std::mutex _metadataProtect;
	static std::mutex _usersProtect;
};

