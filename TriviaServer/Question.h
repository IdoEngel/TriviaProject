#pragma once
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <algorithm>
#include <random>
#include "Entity.h"

constexpr short NUM_OF_TOTAL_ANSWER = 4;

class Question
{
public:
	Question() = default;

	/**
	* @brief gets the ID of the correct answer
	* @returns the ID of the correct answer
	*/
	int getCorrectAnsID() const;

	/**
	* @brief gets all the answers from the class
	* @returns a map of answers a log side the ID of them
	*/
	std::map<unsigned int, std::string> getAnswers();

	/**
	* @brief gets the question
	* @returns the question
	*/
	std::string getQuestion() const;

	/**
	* @brief inserts all the data from the param to the struct in the class
	* @param data a data struct from the db, to take the data from
	*/
	void insert(const QuestionDB& data);
private:

	std::string _question;
	int _correctAnsID;

	std::vector<std::string> _answers;
	std::mutex _answersProtect;
};

