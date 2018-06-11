#include <gamestate/Networker.h>
#include <nlohmann/json.hpp>
#include <mutex>
#include <iostream>
#include <glm/glm.hpp>
#include <gamestate/Entity.h>
#include <events/EventSystem.h>
#include <memory>
#include <gamestate/Command.h>
#include <events/AccumulatedCommandsEvent.h>

using namespace std;

Networker::Networker(EventSystem& _eventSystem)
	: sioSocket{nullptr}
	, eventSystem{_eventSystem}
{
	sioClient.set_reconnect_attempts(reconnectAttempts);

	sioSocket = sioClient.socket();
	sioSocket->on("initState", bind(&Networker::onInitStateReceive, this, placeholders::_1));
	sioSocket->on("state", bind(&Networker::onStateReceive, this, placeholders::_1));
	sioSocket->on("accumulatedCommands", bind(&Networker::onAccumulatedCommandsReceive, this, placeholders::_1));
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

void Networker::sendCommand(uint32_t unitID, std::string action, glm::vec2 direction)
{
	if (!isConnected())
		return;

	sio::message::list arguments;
	arguments.push(sio::string_message::create(std::to_string(unitID)));
	arguments.push(sio::string_message::create(action));
	
	direction = glm::normalize(direction);
	nlohmann::json directionJSON;
	directionJSON["x"] = direction.x;
	directionJSON["y"] = direction.y;
	arguments.push(sio::string_message::create(directionJSON.dump()));

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



Gamestate* Networker::parseGamestate(nlohmann::json state)
{
    
	vector<nlohmann::json> jsonMonsters = state["npcs"];
	vector<nlohmann::json> jsonUnits = state["players"];
	std::map<uint32_t, Unit> units;
	std::map<uint32_t, Monster> monsters;
	
	for (const nlohmann::json& jsonUnit : jsonUnits)
	{
		Unit unit(jsonUnit);
		units.emplace(unit.id(), unit);
	}

	for (const nlohmann::json& jsonMonster : jsonMonsters)
	{
		Monster monster(jsonMonster);
		monsters.emplace(monster.id(), monster);
	}

	/*vector<nlohmann::json> commands = state["commands"];
	for (const nlohmann::json& jsonCommand : commands)
	{
		if (jsonCommand["type"] == std::string("move"))
		{
			uint32_t ID = jsonCommand["ID"];
			nlohmann::json jsonDirection = jsonCommand["direction"];
			glm::vec2 direction(jsonDirection["x"], jsonDirection["y"]);

			if (units.find(ID) != units.end())
			{
				Unit& unit = units.at(ID);
				unit.moveTo(unit.position() + direction);
			}
			if (monsters.find(ID) != monsters.end())
			{
				Monster& monster = monsters.at(ID);
				monster.moveTo(monster.position() + direction);
			}
		}
	}*/
	return new Gamestate(eventSystem, units, monsters);
}


Configuration Networker::parseConfiguration(std::string jsonString)
{
	const nlohmann::json jsonConfig = nlohmann::json::parse(jsonString);
	Configuration config;
	config.sizeX = jsonConfig["width"];
	config.sizeY = jsonConfig["height"];
	return config;
}


std::shared_ptr<Command> Networker::parseCommand(const nlohmann::json& jsonCommand)
{
	std::string type = jsonCommand["type"];
	if (type == "move")
	{
		nlohmann::json direction = jsonCommand["direction"];
		auto command = std::make_shared<MoveCommand>(jsonCommand["ID"], glm::vec2(direction["x"], direction["y"]));
		return command;
	}
	else if (type == "attack")
	{
		nlohmann::json direction = jsonCommand["direction"];
		auto command = std::make_shared<AttackCommand>(jsonCommand["ID"], glm::vec2(direction["x"], direction["y"]));
		return command;
	}
	else if (type == "damage")
	{
		nlohmann::json direction = jsonCommand["direction"];
		auto command = std::make_shared<DamageCommand>(jsonCommand["ID"], glm::vec2(direction["x"], direction["y"]));
		return command;
	}
	else if (type == "die")
	{
		auto command = std::make_shared<DieCommand>(jsonCommand["ID"].get<uint32_t>());
		return command;
	}
	return nullptr;
}

void Networker::processCommands(nlohmann::json& jsonCommands)
{
	std::vector<std::shared_ptr<Command>> commands;
	for (const nlohmann::json& jsonCommand : jsonCommands)
	{
		std::shared_ptr<Command> command = parseCommand(jsonCommand);
		if (command != nullptr)
		{
			commands.push_back(command);
		}
	}
	
	{
		std::lock_guard<std::mutex> queueGuard(queueLock);
		eventQueue.push([=]() {
			for (std::shared_ptr<Command> command : commands)
			{
				CommandEvent event;
				event.command = command;
				eventSystem.processEvent(&event);
			}
		});
	}
}

void Networker::onStateReceive(sio::event _event)
{
	if (!stateCallback) return;

	const string jsonMessage = _event.get_message()->get_string();
	nlohmann::json jsonState = nlohmann::json::parse(jsonMessage);
	Gamestate* state = parseGamestate(jsonState);
		
	{
		lock_guard<mutex> queueGuard(queueLock);
		eventQueue.push([=]() {
			stateCallback(state);
		});
	}

	processCommands(jsonState["commands"]);
}

void Networker::onInitStateReceive(sio::event _event)
{
	if (!initStateCallback) return;
	const string jsonMessage = _event.get_message()->get_string();
	const nlohmann::json initState = nlohmann::json::parse(jsonMessage);
	const Configuration config = parseConfiguration(initState["config"].dump());
	Gamestate* state = parseGamestate(initState["state"]);

	{
		lock_guard<mutex> queueGuard(queueLock);
		eventQueue.push([=]() {
			initStateCallback(config, state);
		});
	}
}

void Networker::onAccumulatedCommandsReceive(sio::event _event)
{
	const std::string jsonMessage = _event.get_message()->get_string();
	nlohmann::json json = nlohmann::json::parse(jsonMessage);
	std::vector<AccumulatedCommands> commandsList;
	
	for (nlohmann::json::iterator it = json.begin(); it != json.end(); it++)
	{
		AccumulatedCommands commands;
		commands.ID = std::stoi(it.key());
		std::vector<nlohmann::json> jsonAttackCommands = it.value()["attack"];
		for (nlohmann::json jsonCommand : jsonAttackCommands)
		{
			commands.attackDirections.push_back(glm::vec2(jsonCommand["x"], jsonCommand["y"]));
		}

		std::vector<nlohmann::json> jsonMoveCommands = it.value()["move"];
		for (nlohmann::json jsonCommand : jsonMoveCommands)
		{
			commands.moveDirections.push_back(glm::vec2(jsonCommand["x"], jsonCommand["y"]));
		}

		commandsList.push_back(commands);
	}
	
	{
		std::lock_guard<std::mutex> queueGuard(queueLock);
		eventQueue.push([=]() {
			AccumulatedCommandsEvent accumulatedCommandsEvent;
			accumulatedCommandsEvent.commands = commandsList;
			eventSystem.processEvent(&accumulatedCommandsEvent);
		});
	}
}
