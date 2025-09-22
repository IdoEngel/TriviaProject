#pragma comment (lib, "ws2_32.lib") //connect the sockets library
#pragma comment(lib, "wininet.lib") //connect to a win-internet library

#include <iostream>
#include "Console.h"
#include "Server.h"
#include "WSAInitializer.h"
#include "FatalException.h"
#include "DB_Exception.h"
#include "JsonResponsePacketSerializer.h"
#include "JsonRequestPacketDeserializer.h"
#include "LoginManager.h"
#include "SqliteDatabase.h"
#include "RoomManager.h"
#include "StatisticsManager.h"
#include "GameManager.h"
#include "Communicator.h"

/**
* @brief cleans static memory of the program - before its end
* @brief BONUS
*/
void cleanStaticMemo();

/**
* @brief main func - entry point of the program
* @param argc the num of args in 'argv'
* @param argv the params to the main func
* @returns 0 - if no errors raised
*/
int main(int argc, const char* argv[]) {
	Console::createFileLog();

	void* m = malloc(4);
	try {
		WSAInitializer wsaInit; //allow socket with safety

		Server ser;

		ser.run();
	}
	catch (const FatalException& e) { //Fatal exception - cannot continue the running
		Console::log(e.what(), Console::LogType::FatalError);

		cleanStaticMemo();
		return 1; //exit with error
	}
	catch (const DB_Exception& e) {
		Console::log(std::string("DB fatal error - ") + e.what(),
			Console::LogType::FatalError);

		cleanStaticMemo();
		return 2; //exit with error
	}

	cleanStaticMemo();
	return 0;
}

void cleanStaticMemo() {
	Console::closeFile();
	Communicator::destroy();
	SqliteDatabase::destroy();
	LoginManager::destroy();
	RoomManager::destroy();
	StatisticsManager::destroy();
	GameManager::destroy();
}
