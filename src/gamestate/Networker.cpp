#include <gamestate/Networker.h>
#include <nlohmann/json.hpp>
#include <mutex>
#include <iostream>

using namespace std;

Networker::Networker()
	: sioSocket{nullptr}
{
	this->sioClient.set_reconnect_attempts(this->reconnectAttempts);
	
	//this->sioSocket->on("initState", bind(&Networker::onInitStateReceive, this, placeholders::_1));
	//this->sioSocket->on("state", bind(&Networker::onStateReceive, this, placeholders::_1));
	this->sioClient.set_open_listener([&]() {
		if (this->connectCallback)
		{
			lock_guard<mutex> eventGuard(queueLock);
			eventQueue.push(this->connectCallback);
		}
	});

	this->sioClient.set_reconnecting_listener([&]() {
		if (this->disconnectCallback)
		{
			lock_guard<mutex> eventGuard(queueLock);
			eventQueue.push(this->disconnectCallback);
		}
	});
}

Networker::~Networker()
{
	this->sioSocket->off_all();
	this->sioClient.sync_close();
}

void Networker::connect(std::string adress, unsigned int port /*= 3000*/)
{
	this->sioClient.connect(string("http://") + adress + ":" + to_string(port));

	this->sioSocket = this->sioClient.socket();
}

bool Networker::isConnected()
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

Gamestate* Networker::parseGamestate(string jsonString)
{
	nlohmann::json state = nlohmann::json::parse(jsonString);

	vector<nlohmann::json> mapObjects = state["mapObjects"];
	vector<Entity> units;
	vector<Entity> monsters;
	
	for (nlohmann::json mapObject: mapObjects)
	{
		Entity entity;
		entity.posX = mapObject["posX"];
		entity.posY = mapObject["posY"];
		string id = mapObject["ID"];
		if (id[0] == 'u')
		{
			units.push_back(entity);
		}
		else
		{
			monsters.push_back(entity);
		}
	}
	return new Gamestate(units, monsters);
}


void Networker::onStateReceive(sio::event _event)
{
	string jsonMessage = _event.get_message()->get_string();
	
}

void Networker::onInitStateReceive(sio::event _event)
{
	string jsonMessage = _event.get_message()->get_string();
	
}
