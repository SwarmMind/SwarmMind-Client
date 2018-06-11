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
	
	void setConnectCallback(std::function<void()> callback);
	void setDisconnectCallback(std::function<void()> callback);
	void setInitStateCallback(std::function<void(Configuration, Gamestate *)> callback);
	void setStateCallback(std::function<void(Gamestate *)> callback);
	void setGameOverCallback(std::function<void()> callback);

	void sendCommand(uint32_t unitID, std::string action, glm::vec2 direction);

	void update();

private:
	class EventSystem& eventSystem;

	sio::client sioClient;
	std::shared_ptr<sio::socket> sioSocket;
	unsigned reconnectAttempts = -1;
	
	std::mutex queueLock;
	std::queue<std::function<void()>> eventQueue;

	//Callbacks
	std::function<void()> connectCallback;
	std::function<void()> disconnectCallback;
	std::function<void(Configuration, Gamestate*)> initStateCallback;
	std::function<void(Gamestate*)> stateCallback;
	std::function<void()> gameOverCallback;

	void onStateReceive(sio::event event);
	void onInitStateReceive(sio::event event);
	void onAccumulatedCommandsReceive(sio::event event);

	Gamestate* parseGamestate(nlohmann::json state);
	Configuration parseConfiguration(std::string jsonString);
	std::shared_ptr<class Command> parseCommand(const nlohmann::json& jsonCommand);
	void processCommands(nlohmann::json& commands);
};
