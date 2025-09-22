#include "Server.h"

Server::Server() : _inputExcPtr(nullptr), _getExit(this->_inputExcPtr),
_commuteClients(Communicator::get()){

}

void Server::run(void) {

	this->_getExit.getAsyncInput();


	while (true) { //accept clients - main loop of the program

		//check if "EXIT" has been typed
		if (this->_inputExcPtr != nullptr) {
			std::rethrow_exception(this->_inputExcPtr);
		}

		this->_commuteClients.startHandleRequest(); //catch one request

	}
}
