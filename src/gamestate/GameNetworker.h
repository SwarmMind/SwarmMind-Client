#pragma once

#include <gamestate/Networker.h>
#include <gamestate/Gamestate.h>
#include <sio_client.h>
#include <string>
#include <memory>
#include <mutex>
#include <functional>
#include <queue>
#include <glm/fwd.hpp>

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
    class EventSystem& eventSystem;

    sio::client sioClient;
    std::shared_ptr<sio::socket> sioSocket;
    unsigned reconnectAttempts = -1;
    unsigned m_reconnectDelay = 100;
	
    void onStateReceive(sio::event event);
    void onInitStateReceive(sio::event event);
    void onAccumulatedCommandsReceive(sio::event event);
    void onChatReceive(sio::event event);

    std::shared_ptr<Gamestate> parseGamestate(nlohmann::json state);
    Configuration parseConfiguration(std::string jsonString);
    std::shared_ptr<class Command> parseCommand(const nlohmann::json& jsonCommand);
    void processCommands(nlohmann::json& commands);
};