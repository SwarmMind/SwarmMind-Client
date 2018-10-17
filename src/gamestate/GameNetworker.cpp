#include <mutex>
#include <iostream>
#include <memory>
#include <set>
#include <exception>

#include <nlohmann/json.hpp>
#include <glm/glm.hpp>

#include <events/EventSystem.h>
#include <events/AccumulatedCommandsEvent.h>
#include <events/InitStateEvent.h>
#include <events/StateEvent.h>
#include <events/DisconnectEvent.h>
#include <events/ChatEvent.h>
#include <events/SoundEvent.h>
#include <sound/Sounds.h>
#include <gamestate/Entity.h>
#include <gamestate/Command.h>
#include <gamestate/ChatEntry.h>

#include <gamestate/GameNetworker.h>

GameNetworker::GameNetworker(EventSystem& eventSystem)
    : m_sioSocket{ nullptr }
    , m_eventSystem{ eventSystem }
{
    m_sioClient.set_reconnect_attempts(m_reconnectAttempts);
    m_sioClient.set_reconnect_delay_max(m_reconnectDelay);

    m_sioSocket = m_sioClient.socket();
	m_sioSocket->on("initState", bind(&GameNetworker::receive, this, ExternalEventType::InitState, std::placeholders::_1));
	m_sioSocket->on("state", bind(&GameNetworker::receive, this, ExternalEventType::State, std::placeholders::_1));
	m_sioSocket->on("accumulatedCommands", bind(&GameNetworker::receive, this, ExternalEventType::AccumulatedCommands, std::placeholders::_1));
	m_sioSocket->on("chat", bind(&GameNetworker::receive, this, ExternalEventType::Chat, std::placeholders::_1));

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

void GameNetworker::receive(const ExternalEventType type, const sio::event event) {
	const std::string jsonMessage = event.get_message()->get_string();
	const nlohmann::json json = nlohmann::json::parse(jsonMessage);

	try {
		switch (type) {
		case ExternalEventType::AccumulatedCommands:
			onAccumulatedCommandsReceive(json);
			break;
		case ExternalEventType::Chat:
			onChatReceive(json);
			break;
		case ExternalEventType::InitState:
			onInitStateReceive(json);
			break;
		case ExternalEventType::State:
			onStateReceive(json);
			break;
		}
	}
	catch (nlohmann::json::out_of_range& e) {
		std::cerr << "Ignoring invalid message: " << e.what() << std::endl;
		return;
	}
}

void from_json(const nlohmann::json& json, Unit& unit) {
	unit = Unit{ json.at("ID"), glm::vec2(json.at("x"), json.at("y")) };
}

void from_json(const nlohmann::json& json, Monster& monster) {
	monster = Monster{ json.at("ID"), glm::vec2(json.at("x"), json.at("y")) };
}

std::shared_ptr<Gamestate> GameNetworker::parseGamestate(nlohmann::json state)
{
	const vector<Monster> jsonMonsters = state.at("npcs").get<std::vector<Monster>>();
	const vector<Unit> jsonUnits = state.at("players").get<std::vector<Unit>>();
    std::map<uint32_t, Unit> units;
    std::map<uint32_t, Monster> monsters;

    for (const auto& unit : jsonUnits)
    {
        units.emplace(unit.id(), unit);
    }

    for (const auto& monster : jsonMonsters)
    {
        monsters.emplace(monster.id(), monster);
    }

    return std::make_shared<Gamestate>(m_eventSystem, units, monsters);
}

void from_json(const nlohmann::json& jsonConfig, Configuration& config)
{
    config.m_sizeX = jsonConfig.at("width");
    config.m_sizeY = jsonConfig.at("height");
	config.m_roundTime = jsonConfig.at("roundTime");
    std::vector<glm::vec2> blockades = jsonConfig.at("blockades");
    config.m_blockadePositions = blockades;
}

std::shared_ptr<Command> GameNetworker::parseCommand(const nlohmann::json& jsonCommand)
{
	static const std::unordered_map<std::string, CommandType> stringToCommand{
		{"move", CommandType::Move},
		{"attack", CommandType::Attack},
		{"damage", CommandType::Damage},
		{"die", CommandType::Die},
		{"spawn", CommandType::Spawn}
	};

	const uint32_t id = jsonCommand.at("ID");
	std::shared_ptr<Command> command;
	switch (stringToCommand.at(jsonCommand.at("type"))) {
	case CommandType::Move:
        command = std::make_shared<MoveCommand>(id, jsonCommand.at("executedMovement"));
		break;
	case CommandType::Attack:
        command = std::make_shared<AttackCommand>(id, jsonCommand.at("direction"));
		break;
    case CommandType::Damage:
        command = std::make_shared<DamageCommand>(id, jsonCommand.at("direction"));
		break;
	case CommandType::Die:
        command = std::make_shared<DieCommand>(id);
		break;
	case CommandType::Spawn:
        command = std::make_shared<SpawnCommand>(id, 
            jsonCommand.at("position").get<glm::vec2>(), 
            jsonCommand.at("isPlayer").get<bool>());
		break;
	default:
		throw nlohmann::json::out_of_range::create(501, "Unknown command type");
    }
	return command;
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

void GameNetworker::onStateReceive(const nlohmann::json& jsonState)
{
    const auto state = parseGamestate(jsonState);
	const auto jsonCommands = jsonState.at("commands");

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

void GameNetworker::onInitStateReceive(const nlohmann::json& initState)
{
	const nlohmann::json jsonConfig = initState.at("config");
    const Configuration config = jsonConfig;
    const auto state = parseGamestate(initState.at("state"));
	const double timeSinceLastRound = jsonConfig.at("timeSinceLastRound");

    m_eventSystem.postEvent(std::make_shared<InitStateEvent>(timeSinceLastRound, config, state));
}

void GameNetworker::onAccumulatedCommandsReceive(const nlohmann::json& json)
{
    nlohmann::json playerCommandsJson = json.at("playerCommands");

    std::vector<AccumulatedCommands> commandsList;
	commandsList.reserve(playerCommandsJson.size());

    for (nlohmann::json::iterator it = playerCommandsJson.begin(); it != playerCommandsJson.end(); it++)
    {
        AccumulatedCommands commands;
        commands.m_ID = std::stoi(it.key());
		commands.m_attackDirections = it.value().at("attack").get<std::vector<glm::vec2>>();
		commands.m_moveDirections = it.value().at("move").get<std::vector<glm::vec2>>();
        commandsList.emplace_back(commands);
    }

    const size_t numberOfGivenCommands = json.at("numberOfGivenCommands");
    const size_t maxNumberOfCommands = json.at("maxNumberOfCommands");

    m_eventSystem.postEvent(std::make_shared<AccumulatedCommandsEvent>(commandsList, numberOfGivenCommands, maxNumberOfCommands));
}

void from_json(const nlohmann::json& json, ChatEntry& chat)
{
    chat.m_user = json.at("userName").get<std::string>();
    chat.m_text = json.at("text").get<std::string>();
    std::string stringPosition = json.at("position");
    nlohmann::json jsonPosition = nlohmann::json::parse(stringPosition);
    chat.m_position = jsonPosition;
}

void GameNetworker::onChatReceive(const nlohmann::json& json)
{
	m_eventSystem.postEvent(std::make_shared<ChatEvent>(ChatEntry{ json }));
}
