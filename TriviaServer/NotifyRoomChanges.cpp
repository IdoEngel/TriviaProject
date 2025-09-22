#include "NotifyRoomChanges.h"
#include "Communicator.h"
#include "RequestHandlerFactory.h"

std::shared_ptr<Room> NotifyRoomChanges::getUsersRoom(const std::string& username, bool adminOnly) {
	RequestHandlerFactory factory;
	std::shared_ptr<Room> usersRoom;

	std::vector<std::shared_ptr<Room>> rooms = factory.getRoomManager().getRooms();
	//for every room
	for (const std::shared_ptr<Room>& room : rooms) {

		//if the user is in this room
		if (room->doesUserInRoom(LoggedUser(username))) {
			//check if take only if user is admin
			if (adminOnly && room->doesUserAdminOfTheRoom(username)) {
				usersRoom = room;
			}
			//if and only if can return non-admin room (room that the user is not the admin of)
			else if (!adminOnly) {
				usersRoom = room;
			}
		}
	}

	return usersRoom;
}

NotifyRoomChanges::NotifyRoomChanges(const std::shared_ptr<Room>& room) : _allClients(Communicator::get()) {
	this->_room = room;
}

void NotifyRoomChanges::triggerCloseRoom() {
	std::optional<SOCKET> currSock = {};
	std::string portNum;

	CloseRoomResponse res;
	Buffer serialized;

	res.status = (unsigned int)Codes::_NOTIFY_CLOSED_ROOM;
	serialized = JsonResponsePacketSerializer::serializeCloseRoomResponse(res, TO_STR);

	const std::vector<LoggedUser> members = this->_room->getAllUsers();
	//for every user
	for (LoggedUser usr : members) {
		currSock = this->_allClients.getClientSock(usr.username);

		//if the socket still exists (client still connects)
		if (currSock.has_value() &&
			//user is not the admin
			!this->_room->doesUserAdminOfTheRoom(usr.username)) {
			portNum = this->_allClients.getSockPort(currSock.value());

			//send the client msg about the room that closed
			this->_allClients.takeAction((int)ClientCodes::_NOTIFY_CLOSED_ROOM_sts, serialized, currSock.value(), portNum);
		}
	}
}

void NotifyRoomChanges::triggerStartGame() {
	std::optional<SOCKET> currSock = {};
	std::string portNum;

	StartGameResponse res;
	Buffer serialized;

	res.status = (unsigned int)Codes::_NOTIFY_GAME_STARTED;
	serialized = JsonResponsePacketSerializer::serializeStartGameResponse(res, TO_STR);

	const std::vector<LoggedUser> members = this->_room->getAllUsers();
	//for every user
	for (LoggedUser usr : members) {
		currSock = this->_allClients.getClientSock(usr.username);

		//if the socket still exists (client still connects)
		if (currSock.has_value() && 
			//user is not the admin
			!this->_room->doesUserAdminOfTheRoom(usr.username)) {
			portNum = this->_allClients.getSockPort(currSock.value());

			//send the client msg about the room that closed
			this->_allClients.takeAction((int)ClientCodes::_NOTIFY_GAME_STARTED_sts, serialized, currSock.value(), portNum);
		}
	}
}

void NotifyRoomChanges::triggerAdminLeftMember(const std::string& memberLeft) {
	std::optional<SOCKET> currSock = {};
	std::string adminUsername; 
	std::string sockNum;

	NotifyAdminAboutLeftMember res;
	Buffer serialized;

	res.status = (int)Codes::_NOTIFY_ADMIN_MEMBER_LEFT;
	res.memberUsername = memberLeft;
	serialized = JsonResponsePacketSerializer::serializeNotifyAdminAboutLeftMemberResponse(res, TO_STR);

	adminUsername = this->_room->getAdmin();
	currSock = this->_allClients.getClientSock(adminUsername);

	if (currSock.has_value()) {
		sockNum = this->_allClients.getSockPort(currSock.value());
		this->_allClients.takeAction((int)ClientCodes::_NOTIFY_ADMIN_THAT_MEMBER_LEFT_sts, serialized, currSock.value(), sockNum);
	}

}
