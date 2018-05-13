#include <gamestate/Networker.h>
#include <nlohmann/json.hpp>
#include <mutex>

using namespace std;

Networker::Networker(std::string adress, unsigned int port /*= 3000*/)
	: sioSocket{nullptr}
	, initialized{false}
	, gameState{nullptr}
{
	this->sioClient.set_reconnect_attempts(this->reconnectAttempts);
	this->sioClient.connect(string("http://") + adress + ":" + to_string(port));

	this->sioSocket = this->sioClient.socket();
	
	this->sioSocket->on("initState", bind(&Networker::onInitStateReceive, this, placeholders::_1));
	this->sioSocket->on("state", bind(&Networker::onStateReceive, this, placeholders::_1));
}

Networker::~Networker()
{
	this->sioSocket->off_all();
	this->sioClient.close();
	delete this->gameState;
	delete this->stateConfiguration;
}

bool Networker::isConnected()
{
	return this->sioClient.opened();
}

void Networker::onStateReceive(sio::event _event)
{
	string jsonMessage = _event.get_message()->get_string();
	nlohmann::json newState = nlohmann::json::parse(jsonMessage);
	this->setState(newState);
}

void Networker::onInitStateReceive(sio::event _event)
{
	string jsonMessage = _event.get_message()->get_string();
	nlohmann::json initState = nlohmann::json::parse(jsonMessage);

	this->setConfiguration(initState["config"]);
	this->setState(initState["state"]);
}

void Networker::setState(nlohmann::json& newState)
{
	lock_guard<mutex> stateLock(gameStateMutex);

	delete gameState;
	gameState = new nlohmann::json();
	*gameState = newState;
}

void Networker::setConfiguration(nlohmann::json& configuration)
{
	lock_guard<mutex> configLock(configurationMutex);

	if (this->stateConfiguration == nullptr)
	{
		this->stateConfiguration = new nlohmann::json();
	}

	*this->stateConfiguration = configuration;
}

/*!
\brief Returns the current Gamestate whenever a new one is available
\return Pointer to the last received Gamestate, if not returned before, otherwise nullptr
		The returned pointer will not be garbage collected, and must be manually deleted!
*/
nlohmann::json* Networker::getCurrentState()
{
	lock_guard<mutex> stateLock(gameStateMutex);

	nlohmann::json* currentState = this->gameState;
	//Needs to be null, as the current State gets deleted, whenever a new state arrives
	//This would also delete the returned state!
	this->gameState = nullptr;

	return currentState;
}

nlohmann::json Networker::getConfiguration()
{
	//lock_guard<mutex> configurationLock(configurationMutex);

	configurationMutex.lock();

	if (this->stateConfiguration == nullptr)
	{
		nlohmann::json emptyConfig;
		return emptyConfig;
	}
	else
	{
		return *this->stateConfiguration;
	}
	configurationMutex.unlock();
}