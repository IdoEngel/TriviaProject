#include "Communicator.h"
#include "RequestHandlerFactory.h"

#include <chrono>

Communicator* Communicator::_instance = nullptr;
const int Communicator::_PORT = 49153;

Communicator& Communicator::get() {
	if (Communicator::_instance == nullptr) {
		Communicator::_instance = new Communicator();
	}

	return *Communicator::_instance;
}

void printClicks() {
	while (true) {
		std::this_thread::sleep_for(std::chrono::seconds(3));

		auto* clicks = MenuRequestHandler::getClicks();

		for (int i = 0; i < 9; i++) {
			std::cout << clicks[i];
		}
	}

}

void Communicator::destroy() {
	delete Communicator::_instance;
	Communicator::_instance = nullptr;
}

Communicator::Communicator() {
	this->_listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	createListen();
	std::thread t(printClicks);
	t.detach();
}

Communicator::~Communicator() {

	//free memo from the clients map
	std::map<SOCKET, ClientData>::iterator it = this->_clients.begin();
	for (it = it; it != this->_clients.end(); ++it) {
		it->second._handler = nullptr;

		closesocket(it->first);
	}

	this->_clients.clear();

}

void Communicator::startHandleRequest() {
	SOCKET tempClientSock = { 0 };
	std::thread tempThread;
	std::optional<std::string> s; 

	tempClientSock = accept(this->_listenSock, NULL, NULL);

	this->insertToClients(tempClientSock, nullptr, /*No value yet*/{});

	tempThread = std::thread(&Communicator::clientHandler, this, std::move(tempClientSock));
	tempThread.detach();
}

std::optional<SOCKET> Communicator::getClientSock(const std::string& username) {
	std::optional<SOCKET> foundSock = {}; //starts as empty

	std::map<SOCKET, ClientData>::iterator it;
	for (it = this->_clients.begin(); it != this->_clients.end(); ++it) {

		std::lock_guard<std::mutex> lock(this->_clientsLock); //protect the data
		if (it->second._username == username) {
			foundSock = it->first;
		}
	}

	return foundSock;
}

void Communicator::createListen(void) {
	struct sockaddr_in data = { 0 };

	data.sin_port = htons(Communicator::_PORT);
	data.sin_family = AF_INET;
	data.sin_addr.s_addr = INADDR_ANY;

	if (bind(this->_listenSock, (struct sockaddr*)&data, sizeof(data)) == SOCKET_ERROR) { //connect port and IP addr
		//if fails..
		throw FatalException("connect the listening sock to port and IP - Failed!");
	}
	if (listen(this->_listenSock, SOMAXCONN) == SOCKET_ERROR) {
		//if fails..
		throw FatalException("creating a listening sock - Failed!");
	}

	Console::log("listening to clients on port " + std::to_string(Communicator::_PORT));
}

void Communicator::clientHandler(SOCKET clientSock) {

	char* bufferAns = nullptr;

	Buffer ansBufferDatatype;

	std::string msgToThrow = "";

	ErrorResponse errRes;
	Buffer errBuffer;
	std::string errStr = "";

	std::string portNum = "";
	int msgCodeRecv = 0;
	int bytesRecved = 0;
	bool clientDisconnected = false;

	std::map<SOCKET, ClientData>::iterator currClientIt = this->_clients.find(clientSock);

	portNum = getSockPort(clientSock);

	Console::log("", Console::LogType::NewClient, portNum);
	if (portNum != "<unknown>") { //if the port found

		//while the client is connected
		while (!clientDisconnected) {
			
			bufferAns = new char[MAX_LEN_OF_MSG];

			//get the msg from the client
			bytesRecved = recv(clientSock, bufferAns, MAX_LEN_OF_MSG, 0);
			if (bytesRecved == SOCKET_ERROR || bytesRecved == 0) {
				clientDisconnected = true;
				msgToThrow = "Error while trying to get msg from client " + portNum;
				break; //go outside the loop
			}

			ansBufferDatatype = JsonResponsePacketSerializer::toBytesBuffer(bufferAns, bytesRecved);
			msgCodeRecv = JsonRequestPacketDeserializer::getMsgCode(ansBufferDatatype);

			try {
				this->checkMsgValidation(currClientIt, ansBufferDatatype); //check msg validation
				//if valid (if not exception), insert into the map the right handler
				updateClientHandler(currClientIt, static_cast<ClientCodes>(msgCodeRecv));

				//now, the right handler is implemented, and all the validation tests passed
				takeAction(msgCodeRecv, ansBufferDatatype, clientSock, portNum);
			}
			//this exception is for error that required the client to re-connect to the server
			catch (const ClientConnectionException& e) {
				Console::log(e.server(), Console::LogType::ClientRequestFaild, portNum);

				//send error msg to client
				errRes.message = e.client();
				errBuffer = JsonResponsePacketSerializer::serializeErrorResponse(errRes, TO_STR);
				errStr = std::string(errBuffer.data.begin(), errBuffer.data.end());
				send(clientSock, errStr.c_str(), errStr.length(), 0);
			}
			catch (const DB_Exception& e) {
				Console::log(e.what(), Console::LogType::DB_Fail);

				/*Send an error back to the client*/
				errRes.message = e.what();
				errBuffer = JsonResponsePacketSerializer::serializeErrorResponse(errRes, TO_STR);
				errStr = std::string(errBuffer.data.begin(), errBuffer.data.end());
				send(clientSock, errStr.c_str(), errStr.length(), 0);
			}

			delete[] bufferAns;
			bufferAns = nullptr;
		}
	}
	else { //if the port not found
		msgToThrow = "Cannot find the port of the user - there for something is not right";
	}

	std::lock_guard<std::mutex> lock(this->_leavesLock); //only one client can leave at a time

	//tell remove from room (if any)
	if (currClientIt->second._username.has_value()) {
		try {
			this->_reqFactory.getRoomManager().releaseUser(currClientIt->second._username.value());
		}
		/*
		* if this exception was thrown, meaning the user has a room.
		* if the user don't have a room, in releaseUser() - if room.get() is nullptr nothing is happening - no Exception
		* if the exception has thrown, either the user is an admin or a member.
		*	need to delete the room if the user is admin.
		* check if admin and if true - delete
		*/
		catch (const ClientConnectionException&) {
			//return room if the user is an admin of the room
			std::shared_ptr<Room> room = NotifyRoomChanges::getUsersRoom(currClientIt->second._username.value(), true);
			//if there is a room
			if (room.get() != nullptr) {
				this->_reqFactory.getRoomManager().deleteRoom(room->getRoomData().id);
			}
		}
		//log out from the manager
		this->_reqFactory.getLoginManager().logout(currClientIt->second._username.value());
	}

	//delete from communicator memo
	this->deleteFromClients(clientSock);
	closesocket(clientSock);

	delete[] bufferAns;
	bufferAns = nullptr;

	Console::log(msgToThrow, Console::LogType::ClientConnectError);
 }


void Communicator::takeAction(const int code, const Buffer& buffer, const SOCKET& sock, const std::string& portNum) {
	Buffer errBuffer;
	std::string msgToClient = "";
	RequestInfo info((size_t)std::stoi(portNum), buffer);
	RequestResult res;

	this->_clients.at(sock)._handler->isRequestRelevant(info, this->_clients.at(sock)); //throw for errors that that the user need to start over the connection
	//if not relevant, throw - there for, if reached to 'handleRequest()' - relevant
	res = this->_clients.at(sock)._handler->handleRequest(info, this->_clients.at(sock));

	msgToClient = std::string(res.buffer.data.begin(), res.buffer.data.end());

	Communicator::printMsgFromTakeAction(code, portNum, res.newHandler == nullptr);

	send(sock, msgToClient.c_str(), msgToClient.length(), 0);
	this->_clients.at(sock)._handler = res.newHandler;
}

void Communicator::printMsgFromTakeAction(const int code, const std::string& port, const bool isHandlerNull) {
	if (code == (int)ClientCodes::LOG_OUT_REQUEST) { //if this is a logout, newHandler need to be nullptr
		Console::log("Action number " + std::to_string(code),
			//get the right console prints
			(isHandlerNull ? Console::LogType::ClientRequestPassed : Console::LogType::ClientRequestFaild),
			port);
	}
	else if (code == (int)ClientCodes::_NOTIFY_CLOSED_ROOM_sts) {
		Console::log("",
				Console::LogType::NotifyAboutClosedRoom_sts,
			port);
	}
	else if (code == (int)ClientCodes::_NOTIFY_GAME_STARTED_sts) {
		Console::log("",
			Console::LogType::NotifyAboutGameStarted_sts,
			port);
	}
	else if (code == (int)ClientCodes::_NOTIFY_ADMIN_THAT_MEMBER_LEFT_sts) {
		Console::log("",
			Console::LogType::NotifyAdminThatMemberLeft_sts,
			port);
	}
	else { //if this not a logout, newHandler should not be nullptr
		Console::log("Action number " + std::to_string(code),
			//get the right console prints
			(!isHandlerNull ? Console::LogType::ClientRequestPassed : Console::LogType::ClientRequestFaild),
			port);
	}
}

bool Communicator::deleteFromClients(const SOCKET& sockKey) noexcept {
	std::lock_guard<std::mutex> lock(this->_clientsLock);

	//delete the data in the map
	std::map<SOCKET, ClientData>::iterator item = this->_clients.find(sockKey);
	item->second._handler = nullptr;

	closesocket(item->first);

	return this->_clients.erase(sockKey) != 0; //return true if the number of elements erased is not 0
}

void Communicator::insertToClients(const SOCKET& sockKey, IRequestHandler* ptrValue, const std::optional<std::string>& username) noexcept {
	std::lock_guard<std::mutex> lock(this->_clientsLock);
	ClientData data;

	data._handler = std::shared_ptr<IRequestHandler>(ptrValue);
	data._username = username;

	this->_clients.emplace(sockKey, data);
}

std::string Communicator::getSockPort(const SOCKET& sock) noexcept {
	std::string portNum = "<unknown>";

	struct sockaddr_in sin;
	int addrlen = sizeof(sin);
	if (getpeername(sock, (struct sockaddr*)&sin, &addrlen) == 0 && //function got information of the sock
		addrlen == sizeof(sin) && //got all the information
		sin.sin_family == AF_INET) // type of connection is correct 
	{
		portNum = std::to_string(ntohs(sin.sin_port));
	}

	return portNum;
}

void Communicator::updateClientHandler(std::map<SOCKET, ClientData>::iterator& client, ClientCodes code) {

	ClientConnectionException::info_t errInfo;

	switch (code)
	{
	case ClientCodes::GENERAL_ERROR:
		break;
	case ClientCodes::SIGN_UP_REQUEST:
		Console::log("sign up to the system", Console::LogType::ClientRequest, getSockPort(client->first));
		client->second._handler = this->_reqFactory.createLoginRequestHandler();
		break;
	case ClientCodes::LOG_IN_REQUEST:
		Console::log("log in to the system", Console::LogType::ClientRequest, getSockPort(client->first));
		client->second._handler = this->_reqFactory.createLoginRequestHandler();
		break;
	case ClientCodes::LOG_OUT_REQUEST:
		Console::log("log out from the system", Console::LogType::ClientRequest, getSockPort(client->first));
		break;
	case ClientCodes::PLAYERS_IN_ROOM_REQUEST:
		Console::log("get players in a room", Console::LogType::ClientRequest, getSockPort(client->first));
		break;
	case ClientCodes::JOIN_ROOM_REQUEST:
		Console::log("join a room", Console::LogType::ClientRequest, getSockPort(client->first));
		break;
	case ClientCodes::CREATE_ROOM_REQUEST:
		Console::log("create a room", Console::LogType::ClientRequest, getSockPort(client->first));
		break;
	case ClientCodes::GET_HIGH_SCORE_REQUEST:
		Console::log("get top 5 players statistics", Console::LogType::ClientRequest, getSockPort(client->first));
		break;
	case ClientCodes::GET_PRSONAL_STAT_REQUEST:
		Console::log("get personal statistics", Console::LogType::ClientRequest, getSockPort(client->first));
		break;
	case ClientCodes::GET_ROOMS_REQUEST:
		Console::log("get room list", Console::LogType::ClientRequest, getSockPort(client->first));
		break;
	case ClientCodes::CLOSE_ROOM_REQUEST:
		Console::log("close a room as an admin", Console::LogType::ClientRequest, getSockPort(client->first));
		break;
	case ClientCodes::START_GAME_REQUEST:
		Console::log("start game in a room as an admin", Console::LogType::ClientRequest, getSockPort(client->first));
		break;
	case ClientCodes::GET_ROOM_STATE_REQUEST:
		Console::log("get room state", Console::LogType::ClientRequest, getSockPort(client->first));
		break;
	case ClientCodes::LEAVE_ROOM_REQUEST:
		Console::log("leave its current room", Console::LogType::ClientRequest, getSockPort(client->first));
		break;
	case ClientCodes::LEAVE_GAME_REQUEST:
		Console::log("leave its current room", Console::LogType::ClientRequest, getSockPort(client->first));
		break;
	case ClientCodes::GET_QUESTION_REQUEST:
		Console::log("get the next question from the game its in", Console::LogType::ClientRequest, getSockPort(client->first));
		break;
	case ClientCodes::SUBMIT_ANS_REQUEST:
		Console::log("submit its answer to the last given question", Console::LogType::ClientRequest, getSockPort(client->first));
		break;
	case ClientCodes::GET_GAME_RESULTS_REQUEST:
		Console::log("get the game results after answering all questions",
			Console::LogType::ClientRequest, getSockPort(client->first));
		break;
	default:
		errInfo.clientMsg = "code " + std::to_string((int)code) + " is not an action on the server";
		errInfo.serverPrint = "The msg code provided by the user is not valid - msg: " + (int)code;
		throw ClientConnectionException(errInfo);
		break;
	}

}

void Communicator::checkMsgValidation(const std::map<SOCKET, ClientData>::iterator& client,
	const Buffer& reqBuffer) {

	ClientConnectionException::info_t errInfo;
	nlohmann::json js;
	bool jsonKeysCheck = false;
	bool handlerNull = false; //false - handler must be not-null. true - handler must be null
	int jsonKeysCounter = 0;

	short msgCode = JsonRequestPacketDeserializer::getMsgCode(reqBuffer);

	//for each msg code - check the relevant items
	switch (static_cast<ClientCodes>(msgCode))
	{
	case ClientCodes::GENERAL_ERROR:
		break;
	case ClientCodes::SIGN_UP_REQUEST:
		handlerNull = true;

		if (JsonRequestPacketDeserializer::checkJsonFormat(reqBuffer, &js)) { //if the format is valid
			for (auto& elem : js.items()) { //check if all the items needed exists
				if (elem.key() == fieldNames::KEY_USERNAME)
					jsonKeysCounter++;
				if (elem.key() == fieldNames::KEY_PASSWORD)
					jsonKeysCounter++;
				if (elem.key() == fieldNames::KEY_EMAIL)
					jsonKeysCounter++;
			}
		}

		jsonKeysCheck = (jsonKeysCounter == fieldCount::SIGN_UP);
		break;
	case ClientCodes::LOG_IN_REQUEST:
		handlerNull = true;

		if (JsonRequestPacketDeserializer::checkJsonFormat(reqBuffer, &js)) { //if the format is valid
			for (auto& elem : js.items()) { //check if all the items needed exists
				if (elem.key() == fieldNames::KEY_USERNAME)
					jsonKeysCounter++;
				if (elem.key() == fieldNames::KEY_PASSWORD)
					jsonKeysCounter++;
			}
		}
		jsonKeysCheck = (jsonKeysCounter == fieldCount::LOG_IN);
		break;
	case ClientCodes::LOG_OUT_REQUEST:
		handlerNull = false;
		
		if (JsonRequestPacketDeserializer::checkJsonFormat(reqBuffer, &js)) {
			for (auto& elem : js.items()) {
				if (elem.key() == fieldNames::KEY_USERNAME)
					jsonKeysCounter++;
			}
		}

		jsonKeysCheck = (jsonKeysCounter == fieldCount::LOG_OUT);
		break;
	case ClientCodes::PLAYERS_IN_ROOM_REQUEST:
		handlerNull = false;

		if (JsonRequestPacketDeserializer::checkJsonFormat(reqBuffer, &js)) {
			for (auto& elem : js.items()) {
				if (elem.key() == fieldNames::KEY_ROOM_ID)
					jsonKeysCounter++;
			}
		}
		jsonKeysCheck = (jsonKeysCounter == fieldCount::PLAYERS_IN_ROOM);
		break;
	case ClientCodes::JOIN_ROOM_REQUEST:
		handlerNull = false;

		if (JsonRequestPacketDeserializer::checkJsonFormat(reqBuffer, &js)) {
			for (auto& elem : js.items()) {
				if (elem.key() == fieldNames::KEY_ROOM_ID)
					jsonKeysCounter++;
				if (elem.key() == fieldNames::KEY_USERNAME)
					jsonKeysCounter++;
			}
		}

		jsonKeysCheck = (jsonKeysCounter == fieldCount::JOIN_ROOM);
		break;
	case ClientCodes::CREATE_ROOM_REQUEST:
		handlerNull = false;

		if (JsonRequestPacketDeserializer::checkJsonFormat(reqBuffer, &js)) {
			for (auto& elem : js.items()) {
				if (elem.key() == fieldNames::KEY_ROOM_NAME)
					jsonKeysCounter++;
				if (elem.key() == fieldNames::KEY_USERNAME)
					jsonKeysCounter++;
				if (elem.key() == fieldNames::KEY_MAX_PLAYERS)
					jsonKeysCounter++;
				if (elem.key() == fieldNames::KEY_NUM_OF_QUESTIONS)
					jsonKeysCounter++;
				if (elem.key() == fieldNames::KEY_TIME_PER_QUESTION)
					jsonKeysCounter++;
			}
		}

		jsonKeysCheck = (jsonKeysCounter == fieldCount::CREATE_ROOM);
		break;
	case ClientCodes::GET_HIGH_SCORE_REQUEST:
		handlerNull = false;
		jsonKeysCheck = true; //no check available or needed
		break;
	case ClientCodes::GET_PRSONAL_STAT_REQUEST:
		handlerNull = false;

		if (JsonRequestPacketDeserializer::checkJsonFormat(reqBuffer, &js)) {
			for (auto& elem : js.items()) {
				if (elem.key() == fieldNames::KEY_USERNAME)
					jsonKeysCounter++;
			}
		}

		jsonKeysCheck = (jsonKeysCounter == fieldCount::GET_PERSONAL_STATS);
		break;
	case ClientCodes::GET_ROOMS_REQUEST:
		handlerNull = false;
		jsonKeysCheck = true;
		break;
	case ClientCodes::CLOSE_ROOM_REQUEST:
		handlerNull = false;
		jsonKeysCheck = true;
		break;
	case ClientCodes::START_GAME_REQUEST:
		handlerNull = false;
		jsonKeysCheck = true;
		break;
	case ClientCodes::GET_ROOM_STATE_REQUEST:
		handlerNull = false;
		jsonKeysCheck = true;
		break;
	case ClientCodes::LEAVE_ROOM_REQUEST:
		handlerNull = false;
		jsonKeysCheck = true;
		break;
	case ClientCodes::LEAVE_GAME_REQUEST:
		handlerNull = false;
		jsonKeysCheck = true;
		break;
	case ClientCodes::GET_QUESTION_REQUEST:
		jsonKeysCheck = true;
		break;
	case ClientCodes::SUBMIT_ANS_REQUEST:
		handlerNull = false;

		if (JsonRequestPacketDeserializer::checkJsonFormat(reqBuffer, &js)) {
			for (auto& elem : js.items()) {
				if (elem.key() == fieldNames::KEY_ANS_ID)
					jsonKeysCounter++;
			}
		}

		jsonKeysCheck = (jsonKeysCounter == fieldCount::SUBMIT_ANSWER);
		break;
	case ClientCodes::GET_GAME_RESULTS_REQUEST:
		handlerNull = false;
		jsonKeysCheck = true;
		break;
	default:
		errInfo.clientMsg = "You sent an invalid code, this action does not exists on the server";
		errInfo.serverPrint = "client sent invalid code";
		throw ClientConnectionException(errInfo);
		break;
	}

	//throw if the number of fields not match the expected count
	if (!jsonKeysCheck) {
		errInfo.clientMsg = "The fields you provided are not the ones needed for the action, check which ones you need";
		errInfo.serverPrint = "fields provided are not valid or missing";
		throw ClientConnectionException(errInfo);
	}

	//if handler needs to be null and its not
	if (handlerNull && client->second._handler.get() != nullptr) {
		errInfo.clientMsg = "you must be logged out to perform this action!";
		errInfo.serverPrint = "client needs to be logged out to perform this action";
		throw ClientConnectionException(errInfo);
	}
	
	if (!handlerNull && client->second._handler.get() == nullptr) {
		errInfo.clientMsg = "you must be logged in to perform this action!";
		errInfo.serverPrint = "client needs to be logged in to perform this action";
		throw ClientConnectionException(errInfo);
	}
}
