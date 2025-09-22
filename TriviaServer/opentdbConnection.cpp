#include "opentdbConnection.h"
#pragma warning(disable : 4996)

bool opentdbConnection::connectToRemote() {
	LPDWORD internetState = 0;
	bool connected = true;

	if (InternetGetConnectedState(internetState, 0)) { //returns TRUE if there is internet connection
		httplib::Client cli(pureURL.data());
		httplib::Result res = cli.Get(questionURL.data());

		try {
			this->_serverAns = res.value();
			if (res.error() != httplib::Error::Success) {
				connected = false;
			}
		}
		catch (const std::exception& e) { //if there is no internt
			connected = false;
		}
	}
	else {
		connected = false;
	}

	return connected;
}


nlohmann::json opentdbConnection::getData() {
	nlohmann::json js;
	nlohmann::json filtered;

	js = nlohmann::json::parse(this->_serverAns.body, nullptr, false);
	//if the parsing went well
	if (js.type() != nlohmann::json::value_t::discarded) {

		std::copy(js["results"].begin(), js["results"].end(), std::back_insert_iterator(filtered));

		for (auto& elem : filtered) {
			elem.erase(jsonFieldNames::categoryDel);
			elem.erase(jsonFieldNames::difficultyDel);
			elem.erase(jsonFieldNames::typeDel);
		}
	}

	return filtered;
}
