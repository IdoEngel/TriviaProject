#pragma once
#include "WSAInitializer.h"

#include <map>
#include <thread>
#include <mutex>
#include <memory>
#include "Console.h"
#include "ClientConnectionException.h"
#include "IRequestHandler.h"
#include "EnumCodes.h"
#include "RequestResponseStructs.h"
#include "JsonResponsePacketSerializer.h"
#include "JsonRequestPacketDeserializer.h"
#include "LoginRequestHandler.h"
#include "RequestHandlerFactory.h"


constexpr const size_t MAX_LEN_OF_MSG = 8192;

/**
* @brief this class is the working class to create clients threads and send msgs to them
*/
class Communicator
{
public:
	Communicator& operator=(const Communicator&) = delete;
	Communicator(const Communicator&) = delete;

	//singleton
	static Communicator& get();

	//singleton
	static void destroy();

	/**
	* @brief free memo and close communications
	*/
	~Communicator();

	/**
	* @brief starts listening to a client, only one at a time
	* @brief wait for single client and add it
	*/
	void startHandleRequest();

	/**
	* @brief gets the socket of the provided username
	* @param username the user to get its socket
	* @returns optionally, the socket  
	*/
	std::optional<SOCKET> getClientSock(const std::string& username);

	/**
	* @brief gets the port of the socket passed
	* @brief if no information found - return "<unknown>"
	* @param sock the sock to find its port number
	* @returns the port number of the socket
	*/
	static std::string getSockPort(const SOCKET& sock) noexcept;

	/**
	* @brief takes an action within the server according to the code and the buffer
	* @param code the code of the msg got from client
	* @param buffer the full msg (including the code) got from the client
	* @param sock the socket of the client sent the msg in buffer
	* @param portNum the number of the port
	* @throw ClientConnectionException if the action is invalid in any way
	*/
	void takeAction(const int code, const Buffer& buffer, const SOCKET& sock, const std::string& portNum);

protected:
	/**
	* @brief checks if the validation of the msg - is the msg valid?
	* @param client iterator to the client spot in the map
	* @param reqBuffer the full request of the user
	* @throw ClientConnectionError if the msg is not valid - with error msg	
	*/
	void checkMsgValidation(const std::map<SOCKET, ClientData>::iterator& client,
		const Buffer& reqBuffer);

private:
	/**
	* @brief creates the instance of the class to communicate with clients
	*/
	Communicator();

	static Communicator* _instance; //singleton

	/**
	* @brief create a listening sock that will accept clients
	* @throws FatalException if the creation of the sock fails
	*/
	void createListen(void);

	/**
	* @brief every client will run in this func, each client will get its thread to run on
	* @param clientSock the socket of the client
	*/
	void clientHandler(SOCKET clientSock);

	/**
	* @brief deletes an item from the _clients field, safely
	* @param sockKey the key to delete from the map
	* @returns is the item deleted?
	*/
	bool deleteFromClients(const SOCKET& sockKey) noexcept;

	/**
	* @brief inserts new value into the _clients field, safely
	* @param sockKey the key value to enter into the map
	* @param username the username of the socket
	* @param prtValue the value to insert to the map
	*/
	void insertToClients(const SOCKET& sockKey, IRequestHandler* ptrValue, const std::optional<std::string>& username) noexcept;

	/**
	* @brief prints the right msg from "takeAction()"
	* @param code the action code sent to "takeAction()"
	* @param port the port of the client
	* @param isHandlerNull 'true' if the handler returns from the state machine is null
	*/
	static void printMsgFromTakeAction(const int code, const std::string& port, const bool isHandlerNull);

	/**
	* @brief update the handler of the client to the code (action passed)
	* @param client the client to change its handler
	* @param code the action code client wanted
	* @throw ClientConnectionError if the code is not a valid code
	*/
	void updateClientHandler(std::map<SOCKET, ClientData>::iterator& client, ClientCodes code);

	static const int _PORT;
	SOCKET _listenSock;

	std::map<SOCKET, ClientData> _clients;
	mutable std::mutex _clientsLock;
	mutable std::mutex _leavesLock; //only one client can leave the server at a time (lost connection leaving)

	RequestHandlerFactory _reqFactory;
};

