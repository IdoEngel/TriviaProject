#pragma once
#include "WSAInitializer.h"

#include <iostream>
#include "Console.h"
#include "FatalException.h"
#include "ClientConnectionException.h"
#include "IRequestHandler.h"
#include "Communicator.h"
#include "RequestHandlerFactory.h"

class Server
{
public:

	/**
	* @brief creates an instance of the class
	*/
	Server();

	/**
	* @brief starts the run of the server
	* @brief with this function - server starts to get clients
	* @throws FatalException if user enters to the console "EXIT"
	*/
	void run(void);

private:

	Console _getExit;
	std::exception_ptr _inputExcPtr = nullptr;

	Communicator& _commuteClients;
	RequestHandlerFactory _reqFactory;
};

