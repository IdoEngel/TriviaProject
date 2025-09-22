#pragma once
#include "WSAInitializer.h"
#include <string>
#include "json.hpp"
#include <iostream>
#include <wininet.h>
#include <algorithm>

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

namespace jsonFieldNames {
	const std::string categoryDel = "category";
	const std::string typeDel = "type";
	const std::string difficultyDel = "difficulty";

	const std::string correctAns = "correct_answer";
	const std::string incorrectAns = "incorrect_answers";
	const std::string question = "question";
}


constexpr const std::string_view questionURL = R"(/api.php?amount=35&category=9&type=multiple)";
constexpr const std::string_view pureURL = R"(https://opentdb.com)";

/**
* @brief connects and grab data from the openTDB server, to get questions
*/
class opentdbConnection
{
public:
	opentdbConnection() = default;
	~opentdbConnection() = default;

	/**
	* @brief connects to the remote server, before pulling the ans
	* @returns 'true' if the connection occurred
	*/
	bool connectToRemote();

	/**
	* @brief gets the data sent from the server, and into a json format
	* @brief the ans is a filtered version of the json, with only the data required 
	* @brief need to call connectToRemote() before
	* @returns the json from the ans
	*/
	nlohmann::json getData();

private:
	httplib::Response _serverAns;
};

