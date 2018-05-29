#include <gamestate/Networker.h>
#include <nlohmann/json.hpp>
#include <mutex>
#include <iostream>

using namespace std;

Networker::Networker()
	: sioSocket{nullptr}
{
	sioClient.set_reconnect_attempts(reconnectAttempts);

	sioSocket = sioClient.socket();
	sioSocket->on("initState", bind(&Networker::onInitStateReceive, this, placeholders::_1));
	sioSocket->on("state", bind(&Networker::onStateReceive, this, placeholders::_1));
	sioSocket->on("gameOver", [=](sio::event _event) {
		if (gameOverCallback)
		{
			lock_guard<mutex> eventGuard(queueLock);
			eventQueue.push(gameOverCallback);
		}
	});
	sioClient.set_open_listener([=]() {
		if (connectCallback)
		{
			lock_guard<mutex> eventGuard(queueLock);
			eventQueue.push(connectCallback);
		}
	});

	sioClient.set_reconnecting_listener([=]() {
		if (disconnectCallback)
		{
			lock_guard<mutex> eventGuard(queueLock);
			eventQueue.push(disconnectCallback);
		}
	});
}

Networker::~Networker()
{
	disconnect();
}

void Networker::connect(std::string adress, unsigned int port /*= 3000*/)
{
	sioClient.connect(string("http://") + adress + ":" + to_string(port));

	sioSocket = sioClient.socket();
}

void Networker::disconnect()
{
	sioSocket->off_all();

	//These two lines are a workaround to fix a bug in Socket.IO, which causes this thread to lock forever,
	//if sioClient.sync_close is called while the sioClient is trying to connect
	//The bug seems to be caused, because sioClient.sync_close is waiting for a thread, which is forever
	//trying to reconnect and never notices, that the client should actually close
	sioClient.set_reconnect_attempts(0);
	sioClient.connect(""); //This will cause the reconnect timer to be reset and the sio thread to be joined
	
	
	sioClient.sync_close();
}

bool Networker::isConnected() const
{
	return sioClient.opened();
}

void Networker::setConnectCallback(std::function<void()> callback)
{
	connectCallback = callback;
}

void Networker::setDisconnectCallback(std::function<void()> callback)
{
	disconnectCallback = callback;
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
	if (!isConnected())
		return;

	sio::message::list arguments;
	arguments.push(sio::string_message::create(unitID));
	arguments.push(sio::string_message::create(action));
	arguments.push(sio::string_message::create(direction));

	sioSocket->emit("command", arguments);
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
	
	for (const nlohmann::json& jsonUnit : jsonUnits)
	{
		Entity unit = jsonUnit;
		units.push_back(unit);
	}

	for (const nlohmann::json& jsonMonster : jsonMonsters)
	{
		Entity monster = jsonMonster;
		monsters.push_back(monster);
	}
	return new Gamestate(units, monsters);
}


Configuration Networker::parseConfiguration(std::string jsonString)
{
	const nlohmann::json jsonConfig = nlohmann::json::parse(jsonString);
	Configuration config;
	config.sizeX = jsonConfig["sizeX"];
	config.sizeY = jsonConfig["sizeY"];
	return config;
}

void Networker::onStateReceive(sio::event _event)
{
	if (!stateCallback) return;

	const string jsonMessage = _event.get_message()->get_string();
	Gamestate* state = parseGamestate(jsonMessage);
		
	{
		lock_guard<mutex> queueGuard(queueLock);
		eventQueue.push([=]() {
			stateCallback(state);
		});
	}
}

void Networker::onInitStateReceive(sio::event _event)
{
	if (!initStateCallback) return;
	const string jsonMessage = _event.get_message()->get_string();
	const nlohmann::json initState = nlohmann::json::parse(jsonMessage);
	const Configuration config = parseConfiguration(initState["config"].dump());
	Gamestate* state = parseGamestate(initState["state"].dump());

	{
		lock_guard<mutex> queueGuard(queueLock);
		eventQueue.push([=]() {
			initStateCallback(config, state);
		});
	}
}
