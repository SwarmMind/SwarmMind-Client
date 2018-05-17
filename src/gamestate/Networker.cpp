#include <gamestate/Networker.h>
#include <nlohmann/json.hpp>
#include <mutex>
#include <iostream>

using namespace std;

Networker::Networker()
	: sioSocket{nullptr}
{
	this->sioClient.set_reconnect_attempts(this->reconnectAttempts);

	sioSocket = sioClient.socket();
	this->sioSocket->on("initState", bind(&Networker::onInitStateReceive, this, placeholders::_1));
	this->sioSocket->on("state", bind(&Networker::onStateReceive, this, placeholders::_1));
	this->sioSocket->on("gameOver", [=](sio::event _event) {
		if (this->gameOverCallback)
		{
			lock_guard<mutex> eventGuard(queueLock);
			eventQueue.push(this->gameOverCallback);
		}
	});
	this->sioClient.set_open_listener([=]() {
		if (this->connectCallback)
		{
			lock_guard<mutex> eventGuard(queueLock);
			eventQueue.push(this->connectCallback);
		}
	});

	this->sioClient.set_reconnecting_listener([=]() {
		if (this->disconnectCallback)
		{
			lock_guard<mutex> eventGuard(queueLock);
			eventQueue.push(this->disconnectCallback);
		}
	});
}

Networker::~Networker()
{
	disconnect();
}

void Networker::connect(std::string adress, unsigned int port /*= 3000*/)
{
	this->sioClient.connect(string("http://") + adress + ":" + to_string(port));

	this->sioSocket = this->sioClient.socket();
}

void Networker::disconnect()
{
	this->sioSocket->off_all();
	this->sioClient.sync_close();
}

bool Networker::isConnected() const
{
	return this->sioClient.opened();
}

void Networker::setConnectCallback(std::function<void()> callback)
{
	this->connectCallback = callback;
}

void Networker::setDisconnectCallback(std::function<void()> callback)
{
	this->disconnectCallback = callback;
}

void Networker::setInitStateCallback(std::function<void(Configuration, Gamestate *)> callback)
{
	initStateCallback = callback;
}

void Networker::setStateCallback(std::function<void(Gamestate *)> callback)
{
	stateCallback = callback;
}

void Networker::setGameOverCallback(std::function<void()> callback)
{
	gameOverCallback = callback;
}

void Networker::sendCommand(std::string unitID, std::string action, std::string direction)
{
	if (!this->isConnected())
		return;

	sio::message::list arguments;
	arguments.push(sio::string_message::create(unitID));
	arguments.push(sio::string_message::create(action));
	arguments.push(sio::string_message::create(direction));

	this->sioSocket->emit("command", arguments);
}

void Networker::update()
{
	lock_guard<mutex> queueGuard(queueLock);
	while (eventQueue.size() > 0)
	{
		function<void()> eventToRun = eventQueue.front();
		eventToRun();
		eventQueue.pop();
	}
}

void from_json(const nlohmann::json& json, Entity& entity)
{
	entity.posX = json["posX"];
	entity.posY = json["posY"];
	string id = json["ID"];
	entity.id = id; //For some reason the id string is necessary to avoid a compiler error
}

Gamestate* Networker::parseGamestate(string jsonString)
{
	nlohmann::json state = nlohmann::json::parse(jsonString);

	vector<nlohmann::json> jsonMonsters = state["npcs"];
	vector<nlohmann::json> jsonUnits = state["units"];
	vector<Entity> units;
	vector<Entity> monsters;
	
	for (nlohmann::json jsonUnit : jsonUnits)
	{
		Entity unit = jsonUnit;
		units.push_back(unit);
	}

	for (nlohmann::json jsonMonster : jsonMonsters)
	{
		Entity monster = jsonMonster;
		monsters.push_back(monster);
	}
	return new Gamestate(units, monsters);
}


Configuration Networker::parseConfiguration(std::string jsonString)
{
	nlohmann::json jsonConfig = nlohmann::json::parse(jsonString);
	Configuration config;
	config.sizeX = jsonConfig["sizeX"];
	config.sizeY = jsonConfig["sizeY"];
	return config;
}

void Networker::onStateReceive(sio::event _event)
{
	if (this->stateCallback)
	{
		string jsonMessage = _event.get_message()->get_string();
		Gamestate* state = this->parseGamestate(jsonMessage);
		
		{
			lock_guard<mutex> queueGuard(queueLock);
			this->eventQueue.push([=]() {
				this->stateCallback(state);
			});
		}
	}
	
}

void Networker::onInitStateReceive(sio::event _event)
{
	if (this->initStateCallback)
	{
		string jsonMessage = _event.get_message()->get_string();
		nlohmann::json initState = nlohmann::json::parse(jsonMessage);
		Configuration config = this->parseConfiguration(initState["config"].dump());
		Gamestate* state = this->parseGamestate(initState["state"].dump());

		{
			lock_guard<mutex> queueGuard(queueLock);
			this->eventQueue.push([=]() {
				this->initStateCallback(config, state);
			});
		}
	}
}
