#include "Console.h"

const std::string Console::_exitKeyword = "EXIT";
std::mutex Console::_printSafe;

std::ostream Console::_fileWrite(nullptr);
std::ofstream Console::_fileObj;

Console::Console(std::exception_ptr& ptr) {
	this->_excPtr = &ptr;
}

void Console::log(const std::string& msg, const Console::LogType& type, const std::string& sockPortNum) noexcept {
	std::lock_guard<std::mutex> lock(Console::_printSafe); //print safely - no overlapping

	switch (type)
	{
	case Console::LogType::FatalError:
		std::clog << msg << std::endl;
		std::clog << "	Cannot continue the running of the server duo this error - exit safely" << std::endl << std::endl;

		Console::_fileWrite << msg << std::endl;
		Console::_fileWrite << "	Cannot continue the running of the server duo this error - exit safely" << std::endl << std::endl;
		break;

	case Console::LogType::Log:
		std::clog << msg << std::endl << std::endl;

		Console::_fileWrite << msg << std::endl << std::endl;
		break;

	case Console::LogType::ClientConnectError:
		std::clog << msg << std::endl;
		std::clog << "	Connection with the client is lost - maybe the window of the client closed?" << std::endl << std::endl;

		Console::_fileWrite << msg << std::endl;
		Console::_fileWrite << "	Connection with the client is lost - maybe the window of the client closed?" << std::endl << std::endl;
		break;

	case Console::LogType::ClientRequest:
		std::clog << "Client " << sockPortNum << ":" << std::endl;
		std::clog << "	Try to: " << msg << std::endl << std::endl;

		Console::_fileWrite << "Client " << sockPortNum << ":" << std::endl;;
		Console::_fileWrite << "	Try to: " << msg << std::endl << std::endl;
		break;

	case Console::LogType::ClientRequestFaild:
		std::clog << "Client " << sockPortNum << ": Failed while trying to take the last requested action" << std::endl;
		if (msg != "")
			std::clog << "	More info: " << msg << std::endl << std::endl;
		else
			std::clog << std::endl;


		Console::_fileWrite << "Client " << sockPortNum << ": Failed while trying to take the last requested action" << std::endl;
		if (msg != "")
			Console::_fileWrite << "	More info: " << msg << std::endl << std::endl;
		else
			Console::_fileWrite << std::endl;
		break;

	case Console::LogType::DB_Fail:
		std::clog << "DB Error: " << std::endl;
		std::clog << "	" << msg << std::endl << std::endl;

		Console::_fileWrite << "DB Error: " << std::endl;
		Console::_fileWrite << "	" << msg << std::endl << std::endl;
		break;

	case Console::LogType::ClientRequestPassed:
		std::clog << "Client " << sockPortNum << " Did an action." << std::endl;
		std::clog << "	The action: " << msg << std::endl << std::endl;

		Console::_fileWrite << "Client " << sockPortNum << " Did an action." << std::endl;
		Console::_fileWrite << "	The action: " << msg << std::endl << std::endl;
		break;

	case Console::LogType::NewClient:
		std::clog << "New client connection: " << sockPortNum << std::endl << std::endl;

		Console::_fileWrite << "New client connection: " << sockPortNum << std::endl << std::endl;
		break;

	case Console::LogType::NotifyAboutClosedRoom_sts:
		std::clog << "Server notifying client with sock number: " << sockPortNum << std::endl;
		std::clog << "	The room of the client closed" << std::endl << std::endl;

		Console::_fileWrite << "Server notifying client with sock number: " << sockPortNum << std::endl;
		Console::_fileWrite << "	The room of the client closed" << std::endl << std::endl;
		break;

	case Console::LogType::NotifyAboutGameStarted_sts:
		std::clog << "Server notifying client with sock number: " << sockPortNum << std::endl;
		std::clog << "	The game played in the room of the client begun" << std::endl << std::endl;

		Console::_fileWrite << "Server notifying client with sock number: " << sockPortNum << std::endl;
		Console::_fileWrite << "	The game played in the room of the client begun" << std::endl << std::endl;
		break;

	case Console::LogType::NotifyAdminThatMemberLeft_sts:
		std::clog << "Server notifying client with sock number: " << sockPortNum << std::endl;
		std::clog << "	One of the members left the client's admin room" << std::endl << std::endl;

		Console::_fileWrite << "Server notifying client with sock number: " << sockPortNum << std::endl;
		Console::_fileWrite << "	One of the members left the client's admin room" << std::endl << std::endl;
	}
}

void Console::createFileLog() {
	std::time_t now = std::time(nullptr);
	char nowStr[LEN_OF_TIME_STR] = { 0 };

	Console::_fileObj.open("Trivia.log", std::ios::app );

	//re-direct _fileWrite to write to the file
	std::streambuf* fileDirection = Console::_fileObj.rdbuf();
	Console::_fileWrite.rdbuf(fileDirection);

	ctime_s(nowStr, LEN_OF_TIME_STR - 1, &now);
	Console::_fileWrite << "---- New server run started at: " << nowStr << std::endl;
}

void Console::closeFile() {
	Console::_fileWrite << std::endl << std::endl;
	Console::_fileObj.close();
}

void Console::input(std::string& str, const std::string& msg) noexcept {
	Console::log(msg, LogType::Log);
	std::getline(std::cin, str);
}

void Console::input(int& num, const std::string& msg) {
	Console::log(msg, LogType::Log);

	std::cin >> num;
	if (std::cin.fail()) { // Checks whether an error occurred

		std::cin.clear();   // Clear the fail state
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore the rest of the line (up to a reasonable limit)
		throw InputException("Cannot get input - not a number");
	}          
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 

}

void Console::input(float& num, const std::string& msg) {
	Console::log(msg, LogType::Log);

	std::cin >> num;
	if (std::cin.fail()) { // Checks whether an error occurred

		std::cin.clear();   // Clear the fail state
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore the rest of the line (up to a reasonable limit)
		throw InputException("Cannot get input - not a float number");
	}
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void Console::getAsyncInput() {

	this->_input = std::thread(&Console::async_input,this);
	this->_input.detach();
}

void Console::async_input() {

	while (this->_msgGot != Console::_exitKeyword) {
		Console::input(this->_msgGot);

		if (this->_msgGot == Console::_exitKeyword) {
			try {
				throw FatalException("You wanted to exit");
			}
			catch (...) {
				*this->_excPtr = std::current_exception();
			}
		}
	}

}
