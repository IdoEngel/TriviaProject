#pragma once
#include "Room.h"
#include "Game.h"

class Communicator;

constexpr const bool ADMIN_ONLY = true;
constexpr const bool ALL_MEMBERS = false;

/**
* @brief this class will collect all the actions needed to be sent to all of the members of the room
* @brief the class will send the messages to all of the members of the room
*/
class NotifyRoomChanges
{
public:
	/**
	* @brief gets the room of the user, with its username
	* @param username the user to find its room
	* @param adminOnly return the room only if the username is the admin of the room (use ADMIN_ONLY or ALL_MEMBERS)
	* @returns std::shared_ptr<> of Room (the room of the user)
	*/
	static std::shared_ptr<Room> getUsersRoom(const std::string& username, bool adminOnly);

	/**
	* @brief CONSTRACTOR
	* @param room the room of the current client
	*/
	NotifyRoomChanges(const std::shared_ptr<Room>& room);

	/**
	* @brief notify all the room members that the room was closed
	*/
	void triggerCloseRoom();

	/**
	* @brief notify all the room members that the game in the room begun
	*/
	void triggerStartGame();

	/**
	* @brief general function that sending msg to the admin of the room
	* @param memberLeft the member left the room (its username)
	*/
	void triggerAdminLeftMember(const std::string& memberLeft);

private:
	Communicator& _allClients;
	std::shared_ptr<Room> _room;
};

