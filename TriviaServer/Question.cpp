#include "Question.h"

int Question::getCorrectAnsID() const {
	return this->_correctAnsID;
}

std::map<unsigned int, std::string> Question::getAnswers() {
	int currID = 1;
	std::map<unsigned int, std::string> answersMap;

	std::lock_guard<std::mutex> lock(this->_answersProtect);

	//for every answer in the vector
	for (const std::string& ans : this->_answers) {
		answersMap.emplace(currID, ans);
		currID++;
	}

	return answersMap;
}

std::string Question::getQuestion() const {
	return this->_question;
}

void Question::insert(const QuestionDB& data) {
	this->_answers.clear(); //clear the vector

	//make sure the shuffle will be difference every time
	//https://stackoverflow.com/questions/6926433/how-to-shuffle-a-stdvector

	auto rd = std::random_device{};
	auto random = std::default_random_engine{ rd() };

	this->_question = data.question;
	
	std::lock_guard<std::mutex> lock(this->_answersProtect);
	this->_answers.push_back(data.correct);
	//insert all the incorrect answers
	int i = 0;
	for (i = 0; i < NUM_OF_INCORRECT_ANSWERS; i++) {
		this->_answers.push_back(data.incorrect[i]);
	}

	//shuffle the answers
	std::shuffle(std::begin(this->_answers), std::end(this->_answers), random);

	//after shuffling, find the index of the correct answer
	this->_correctAnsID = std::distance(this->_answers.begin(), //the distance between the first element and the correct ans
		std::find(this->_answers.begin(), this->_answers.end(), data.correct)) + 1; //find the correct ans iterator, add one (index)
}
