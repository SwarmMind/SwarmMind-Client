#include <gamestate/GameNetworker.h>
#include <nlohmann/json.hpp>
#include <mutex>
#include <iostream>
#include <glm/glm.hpp>
#include <gamestate/Entity.h>
#include <events/EventSystem.h>
#include <memory>
#include <gamestate/Command.h>
#include <events/AccumulatedCommandsEvent.h>
#include <events/InitStateEvent.h>
#include <events/StateEvent.h>
#include <events/DisconnectEvent.h>
#include <events/ChatEvent.h>
#include <gamestate/ChatEntry.h>

using namespace std;

GameNetworker::GameNetworker(EventSystem& eventSystem)
    : m_sioSocket{ nullptr }
    , m_eventSystem{ eventSystem }
{
    m_sioClient.set_reconnect_attempts(m_reconnectAttempts);
    m_sioClient.set_reconnect_delay_max(m_reconnectDelay);

    m_sioSocket = m_sioClient.socket();
    m_sioSocket->on("initState", bind(&GameNetworker::onInitStateReceive, this, std::placeholders::_1));
    m_sioSocket->on("state", bind(&GameNetworker::onStateReceive, this, std::placeholders::_1));
    m_sioSocket->on("accumulatedCommands", bind(&GameNetworker::onAccumulatedCommandsReceive, this, std::placeholders::_1));
    m_sioSocket->on("chat", bind(&GameNetworker::onChatReceive, this, std::placeholders::_1));

    m_sioClient.set_reconnecting_listener([=]() {
        m_eventSystem.postEvent(std::make_shared<DisconnectEvent>());
    });
}

GameNetworker::~GameNetworker()
{
    disconnect();
}

void GameNetworker::connect(std::string adress, unsigned int port /*= 3000*/)
{
    m_sioClient.connect(string("http://") + adress + ":" + to_string(port));

    m_sioSocket = m_sioClient.socket();
}

void GameNetworker::disconnect()
{
    m_sioSocket->off_all();

    //These two lines are a workaround to fix a bug in Socket.IO, which causes this thread to lock forever,
    //if sioClient.sync_close is called while the sioClient is trying to connect
    //The bug seems to be caused, because sioClient.sync_close is waiting for a thread, which is forever
    //trying to reconnect and never notices, that the client should actually close
    m_sioClient.set_reconnect_attempts(0);
    m_sioClient.connect(""); //This will cause the reconnect timer to be reset and the sio thread to be joined


    m_sioClient.sync_close();
}

bool GameNetworker::isConnected() const
{
    return m_sioClient.opened();
}

namespace glm
{
    void to_json(nlohmann::json& json, const glm::vec2& vector)
    {
        json["x"] = vector.x;
        json["y"] = vector.y;
    }

    void from_json(const nlohmann::json& json, glm::vec2& vector)
    {
        vector.x = json["x"];
        vector.y = json["y"];
    }
}

void GameNetworker::sendCommand(uint32_t unitID, std::string action, glm::vec2 direction)
{
    if (!isConnected())
        return;

    sio::message::list arguments;
    arguments.push(sio::string_message::create(std::to_string(unitID)));
    arguments.push(sio::string_message::create(action));

    direction = glm::normalize(direction);
    nlohmann::json directionJSON = direction;
    arguments.push(sio::string_message::create(directionJSON.dump()));

    m_sioSocket->emit("command", arguments);
}

void GameNetworker::sendChatMessage(struct ChatEntry& chatEntry)
{
    if (!isConnected())
        return;

    sio::message::list arguments;
    arguments.push(sio::string_message::create(chatEntry.m_user));
    arguments.push(sio::string_message::create(chatEntry.m_text));

    nlohmann::json position = chatEntry.m_position;
    arguments.push(sio::string_message::create(position.dump()));

    m_sioSocket->emit("chat", arguments);
}

void GameNetworker::update(double deltaTime, double timeStamp)
{
}

std::shared_ptr<Gamestate> GameNetworker::parseGamestate(nlohmann::json state)
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
    return std::make_shared<Gamestate>(m_eventSystem, units, monsters);
}

Configuration GameNetworker::parseConfiguration(std::string jsonString)
{
    const nlohmann::json jsonConfig = nlohmann::json::parse(jsonString);
    Configuration config;
    config.m_sizeX = jsonConfig["width"];
    config.m_sizeY = jsonConfig["height"];
    config.m_roundTime = jsonConfig["roundTime"];
    std::vector<glm::vec2> blockades = jsonConfig["blockades"];
    config.m_blockadePositions = blockades;
    return config;
}

std::shared_ptr<Command> GameNetworker::parseCommand(const nlohmann::json& jsonCommand)
{
    std::string type = jsonCommand["type"];
    if (type == "move")
    {
        nlohmann::json direction = jsonCommand["executedMovement"];
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
    else if (type == "spawn")
    {
        auto command = std::make_shared<SpawnCommand>(jsonCommand["ID"].get<uint32_t>(), 
            jsonCommand["position"].get<glm::vec2>(), 
            jsonCommand["isPlayer"].get<bool>());
        return command;
    }
    return nullptr;
}

static const double offset(const CommandType& type) {
	switch (type) {
	case CommandType::Spawn:
		return 0.4;
	case CommandType::Move:
		return 0.3;
	case CommandType::Attack:
	case CommandType::Damage:
		return 0.0;
	case CommandType::Die:
		return 0.1;
	}
	return 0.0;
}

std::vector<std::shared_ptr<Command>> GameNetworker::processCommands(const nlohmann::json& jsonCommands)
{
	std::vector<std::shared_ptr<Command>> commands;
    for (const nlohmann::json& jsonCommand : jsonCommands)
    {
        std::shared_ptr<Command> command = parseCommand(jsonCommand);
		if (command) {
			commands.emplace_back(command);
        }
    }
	return commands;
}

void GameNetworker::onStateReceive(sio::event event)
{
    const string jsonMessage = event.get_message()->get_string();
    const nlohmann::json jsonState = nlohmann::json::parse(jsonMessage);
    const auto state = parseGamestate(jsonState);
	const auto jsonCommands = jsonState["commands"];


	std::vector<TimedEvent> events;
	std::set<CommandType> processedCommandTypes;
	events.reserve(jsonCommands.size() + 1);
	events.emplace_back(std::make_shared<StateEvent>(state));
	for (const nlohmann::json& jsonCommand : jsonCommands)
	{
		if (std::shared_ptr<Command> command = parseCommand(jsonCommand)) {
			processedCommandTypes.emplace(command->type());
			events.emplace_back(std::make_shared<CommandEvent>(command), offset(command->type()));
		}
	}
	events.reserve(events.size() + processedCommandTypes.size());
	for (const auto& type : processedCommandTypes) {
		switch (type) {
		case CommandType::Move:
			events.emplace_back(std::make_shared<SoundEvent>(SoundEnum::Walk), offset(type));
			break;
		case CommandType::Attack:
			events.emplace_back(std::make_shared<SoundEvent>(SoundEnum::Attack), offset(type));
			break;
		case CommandType::Damage:
			events.emplace_back(std::make_shared<SoundEvent>(SoundEnum::Hit), offset(type));
			break;
		case CommandType::Die:
			events.emplace_back(std::make_shared<SoundEvent>(SoundEnum::Die), offset(type));
			break;
		default:
			break;
		}
	}

	m_eventSystem.postEvents(events);
}

void GameNetworker::onInitStateReceive(sio::event event)
{
    const string jsonMessage = event.get_message()->get_string();
    const nlohmann::json initState = nlohmann::json::parse(jsonMessage);
    const Configuration config = parseConfiguration(initState["config"].dump());
    auto state = parseGamestate(initState["state"]);
    double timeSinceLastRound = initState["config"]["timeSinceLastRound"];

    m_eventSystem.postEvent(std::make_shared<InitStateEvent>(timeSinceLastRound, config, state));
}

void GameNetworker::onAccumulatedCommandsReceive(sio::event event)
{
    const std::string jsonMessage = event.get_message()->get_string();
    nlohmann::json json = nlohmann::json::parse(jsonMessage);
    nlohmann::json playerCommandsJson = json["playerCommands"];
    std::vector<AccumulatedCommands> commandsList;

    for (nlohmann::json::iterator it = playerCommandsJson.begin(); it != playerCommandsJson.end(); it++)
    {
        AccumulatedCommands commands;
        commands.m_ID = std::stoi(it.key());
        std::vector<nlohmann::json> jsonAttackCommands = it.value()["attack"];
        for (nlohmann::json jsonCommand : jsonAttackCommands)
        {
            commands.m_attackDirections.emplace_back(jsonCommand["x"], jsonCommand["y"]);
        }

        std::vector<nlohmann::json> jsonMoveCommands = it.value()["move"];
        for (nlohmann::json jsonCommand : jsonMoveCommands)
        {
            commands.m_moveDirections.emplace_back(jsonCommand["x"], jsonCommand["y"]);
        }

        commandsList.push_back(commands);
    }

    size_t numberOfGivenCommands = json["numberOfGivenCommands"];
    size_t maxNumberOfCommands = json["maxNumberOfCommands"];

    m_eventSystem.postEvent(std::make_shared<AccumulatedCommandsEvent>(commandsList, numberOfGivenCommands, maxNumberOfCommands));
}

void from_json(const nlohmann::json& json, ChatEntry& chat)
{
    chat.m_user = json["userName"].get<std::string>();
    chat.m_text = json["text"].get<std::string>();
    std::string stringPosition = json["position"];
    nlohmann::json jsonPosition = nlohmann::json::parse(stringPosition);
    chat.m_position = jsonPosition;
}

void GameNetworker::onChatReceive(sio::event event)
{
    std::string message = event.get_message()->get_string();
    nlohmann::json jsonChat = nlohmann::json::parse(message);
    
	m_eventSystem.postEvent(std::make_shared<ChatEvent>(ChatEntry{ jsonChat }));
}
