#pragma once

#include <gamestate/Gamestate.h>
#include <sio_client.h>
#include <string>
#include <memory>
#include <mutex>
#include <functional>
#include <queue>
#include <glm/fwd.hpp>

class Networker
{
public:
	Networker(class EventSystem& _eventSystem);

	~Networker();

	void connect(std::string adress, unsigned int port = 3000);
	void disconnect();
	bool isConnected() const;
	
	void sendCommand(uint32_t unitID, std::string action, glm::vec2 direction);
    void sendChatMessage(struct ChatEntry& chatEntry);

	void update();

private:
	class EventSystem& eventSystem;

	sio::client sioClient;
	std::shared_ptr<sio::socket> sioSocket;
	unsigned reconnectAttempts = -1;
    unsigned m_reconnectDelay = 100;
	
	std::mutex queueLock;
	std::queue<std::function<void()>> eventQueue;

	void onStateReceive(sio::event event);
	void onInitStateReceive(sio::event event);
	void onAccumulatedCommandsReceive(sio::event event);
    void onChatReceive(sio::event event);

	Gamestate* parseGamestate(nlohmann::json state);
	Configuration parseConfiguration(std::string jsonString);
	std::shared_ptr<class Command> parseCommand(const nlohmann::json& jsonCommand);
	void processCommands(nlohmann::json& commands);
};
