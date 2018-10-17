#pragma once

#include <string>
#include <memory>
#include <mutex>
#include <functional>
#include <queue>

#include <glm/fwd.hpp>
#include <sio_client.h>
#include <nlohmann/json_fwd.hpp>

#include <gamestate/Networker.h>
#include <gamestate/Gamestate.h>

class GameNetworker : public Networker
{
public:
    GameNetworker(class EventSystem& _eventSystem);

    ~GameNetworker();

    void connect(std::string adress, unsigned int port = 3000);
    void disconnect();
    bool isConnected() const;

    virtual void sendCommand(uint32_t unitID, std::string action, glm::vec2 direction) override;
    virtual void sendChatMessage(struct ChatEntry& chatEntry) override;

    virtual void update(double deltaTime, double timeStamp) override;

private:
	enum class ExternalEventType {
		State, InitState, AccumulatedCommands, Chat
	};

    class EventSystem& m_eventSystem;

    sio::client m_sioClient;
    std::shared_ptr<sio::socket> m_sioSocket;
    unsigned m_reconnectAttempts = -1;
    unsigned m_reconnectDelay = 100;
	
    void onStateReceive(const nlohmann::json& json);
    void onInitStateReceive(const nlohmann::json& json);
    void onAccumulatedCommandsReceive(const nlohmann::json& json);
    void onChatReceive(const nlohmann::json& json);

	void receive(const ExternalEventType type, const sio::event event);

    std::shared_ptr<Gamestate> parseGamestate(nlohmann::json state);
    Configuration parseConfiguration(nlohmann::json jsonConfig);
    std::shared_ptr<class Command> parseCommand(const nlohmann::json& jsonCommand);
    std::vector<std::shared_ptr<Command>> processCommands(const nlohmann::json& commands);
};