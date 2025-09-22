#include "LoginRequestHandler.h"

LoginRequestHandler::LoginRequestHandler() : _manager(LoginManager::get()), _db(SqliteDatabase::get()) {}


void LoginRequestHandler::isRequestRelevant(const RequestInfo& reqInfo, ClientData& data) {
	bool isRelevant = true;
	ClientConnectionException::info_t errInfo;

	std::string msgToThrow = "";
	SignupRequest signup;
	LoginRequest login;

	std::regex emailCheck; //BONUS
	std::regex passwordCheckUpper;
	std::regex passwordCheckLower;
	std::regex passwordCheckNums;
	std::regex passwordCheckSymbols;

	switch ((ClientCodes)JsonRequestPacketDeserializer::getMsgCode(reqInfo.buffer))
	{
	case ClientCodes::SIGN_UP_REQUEST:
		signup = JsonRequestPacketDeserializer::deserializeSignupRequest(reqInfo.buffer);

		emailCheck = std::regex(R"([a-zA-Z0-9_]+@[a-zA-Z0-9_]+\.[a-z]+)"); //regex of <any>@<any>.<any>

		passwordCheckUpper = std::regex(R"([A-Z])");
		passwordCheckLower = std::regex(R"([a-z])");
		passwordCheckNums = std::regex(R"(\d)");
		passwordCheckSymbols = std::regex(R"([!@#$%^&*()])");

		//check if all of the symbols are in the password
		if (!std::regex_search(signup.password, passwordCheckUpper) || //uppercase check
			!std::regex_search(signup.password, passwordCheckLower) || //lowercase check
			!std::regex_search(signup.password, passwordCheckNums) || //nums check
			!std::regex_search(signup.password, passwordCheckSymbols)) { //symbols check
			isRelevant = false;
			msgToThrow = "Password must have lower and upper case letters, number, and special character";
		}

		if (!std::regex_match(signup.email, emailCheck) /*check if the email match the regex*/) {
			isRelevant = false;
			msgToThrow = "Email address provided not aliened with the conventions of email addresses pattern";
		}

		//if the user exists OR the email not in the format
		if (this->_db.doesUserExist(signup.username)) {
			isRelevant = false;
			msgToThrow = "User with the same username already in the system";
		}

		//check if the username field is not empty
		if (signup.username.length() == 0) {
			isRelevant = false;
			msgToThrow = "username not provided";
		}
		break;

	case ClientCodes::LOG_IN_REQUEST:
		login = JsonRequestPacketDeserializer::deserializeLoginRequest(reqInfo.buffer);

		if (this->_manager.isLoggedIn(login.username)) {
			isRelevant = false;
			msgToThrow = "You logged in already";
		}

		if (!this->_db.doesPasswordMatch(login.username, login.password)) {
			isRelevant = false;
			msgToThrow = "Password provided not match with the username";
		}
		break;
	default:
		errInfo.clientMsg = "You need to log in to the system before doing any other action";
		errInfo.serverPrint = "Client tried to perform action before logging in";
		throw ClientConnectionException(errInfo);
		break;
	}

	if (!isRelevant) {//if irrelevant
		//fix handler to be null
		data._handler = nullptr;

		errInfo.clientMsg = msgToThrow;
		errInfo.serverPrint = msgToThrow;
		throw ClientConnectionException(errInfo);
	}
}

RequestResult LoginRequestHandler::handleRequest(const RequestInfo& reqInfo, ClientData& cliData) {
	RequestResult res;

	switch ((ClientCodes)JsonRequestPacketDeserializer::getMsgCode(reqInfo.buffer))
	{
	case ClientCodes::SIGN_UP_REQUEST:
		res = this->signup(reqInfo, cliData);
		break;
	case ClientCodes::LOG_IN_REQUEST:
		res = this->login(reqInfo, cliData);
		break;
	case ClientCodes::LOG_OUT_REQUEST:
		break;
	}

	return res;
}

RequestResult LoginRequestHandler::login(const RequestInfo & reqInfo, ClientData& cliData) {
	LoginRequest login;

	LoginResponse resLogin;
	ErrorResponse err; //if error occurred

	RequestResult result; //what returned
	bool success = false;

	login = JsonRequestPacketDeserializer::deserializeLoginRequest(reqInfo.buffer);
	success = this->_manager.login(login.username, login.password);

	//store status code of operation
	resLogin.status = (int)(success ? Codes::LOG_IN_SUCCESS : Codes::LOG_IN_FAIL);
	err.message = "Username do not match the password";
	
	if (success) {
		result.buffer = JsonResponsePacketSerializer::serializeLoginResponse(resLogin, TO_STR);
		result.newHandler = std::make_shared<MenuRequestHandler>(); /*ADD the right element*/
		cliData._username = login.username;
	}
	else {
		this->_manager.logout(login.username);
		result.buffer = JsonResponsePacketSerializer::serializeErrorResponse(err, TO_STR);
		result.newHandler = nullptr;
		cliData._username = {};
	}

	return result;
}

RequestResult LoginRequestHandler::signup(const RequestInfo& reqInfo, ClientData& cliData) {
	SignupRequest signup;

	SignupResponse resSignup;
	ErrorResponse err; //if error occurred

	RequestResult result;
	RequestResult loginRes;
	bool success = false;

	signup = JsonRequestPacketDeserializer::deserializeSignupRequest(reqInfo.buffer);
	success = this->_manager.signup(signup.username, signup.password, signup.email);

	//store status code of operation
	resSignup.status = (int)(success ? Codes::SIGN_UP_SUCCESS : Codes::SIGN_UP_FAIL);
	err.message = "User with the same name already exists";

	result.buffer = JsonResponsePacketSerializer::serializeSignUpResponse(resSignup, TO_STR);
	if (success) {
		loginRes = login(reqInfo, cliData);
		result.newHandler = loginRes.newHandler;
	}
	else {
		result.buffer = JsonResponsePacketSerializer::serializeErrorResponse(err, TO_STR);
		result.newHandler = nullptr;
	}

	return result;
}
