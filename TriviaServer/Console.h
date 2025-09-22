#pragma once
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <fstream>   
#include <ctime>

#include "FatalException.h"
#include "InputException.h"

constexpr const int LEN_OF_TIME_STR = 1024;

/**
* @brief Console class writes, reads and interacts with the console
*/
class Console
{
public:

	/**
	* @brief all the log types that the class can handle
	*/
	enum class LogType {
		FatalError,
		Log,
		ClientConnectError,
		ClientRequest, //sockPortNum is needed
		ClientRequestFaild, //sockPortNum is needed
		DB_Fail, 
		ClientRequestPassed, //sockPortNum is needed
		NewClient, //sockPortNum is needed, msg NOT needed
		NotifyAboutClosedRoom_sts, //msg NOT needed, server to server
		NotifyAboutGameStarted_sts, //msg NOT needed, server to server
		NotifyAdminThatMemberLeft_sts, //msg NOT needed, server to server
	};

	/**
	* @brief creates an instance of the class
	* @param ptr the exception ptr to get the exception from
	*/
	Console(std::exception_ptr& ptr);

	/**
	* @brief gets an input async, so the input will be able to catch at all time
	*/
	void getAsyncInput();

	/**
	* @brief gets an input from the console - std::string type
	* @param str the string that will get the input
	* @param msg msg to print before getting input (default: empty string)
	*/
	static void input(std::string& str, const std::string& msg = "") noexcept;

	/**
	* @brief gets an input from the console - std::string type
	* @param str the string that will get the input
	* @param msg msg to print before getting input (default: empty string)
	* @throws InputException if input did not go as excepted
	*/
	static void input(int& num, const std::string& msg = "");

	/**
	* @brief gets an input from the console - std::string type
	* @param str the string that will get the input
	* @param msg msg to print before getting input (default: empty string)
	* @throws InputException if input did not go as excepted
	*/
	static void input(float& num, const std::string& msg = "");

	/**
	* @brief log msg to the console, with special format, according to the type
	* @param msg the msg to print
	* @param type the type of the msg to print (may add format or more info)
	* @param sockPortNum the number of the port of the client - to log for specific client
	*/
	static void log(const std::string& msg, const LogType& type = Console::LogType::Log, const std::string& sockPortNum = "<unpassed>") noexcept;

	/**
	* @brief create a file log so writing to the console also write to a file
	*/
	static void createFileLog();

	/**
	* @brief close the file after finished running the program
	*/
	static void closeFile();
private:

	/**
	* @brief the function that will run async and wait for input
	*/
	void async_input();

	static const std::string _exitKeyword;
	std::thread _input; //thread to accept input from console
	std::string _msgGot;
	std::exception_ptr* _excPtr;

	static std::ostream _fileWrite;
	static std::ofstream _fileObj;

	static std::mutex _printSafe;
};

