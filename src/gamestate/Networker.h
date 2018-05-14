#pragma once

#include <gamestate/Gamestate.h>
#include <sio_client.h>
#include <string>
#include <memory>
#include <mutex>
#include <functional>
#include <queue>

class Networker
{
public:
	Networker();

	~Networker();

	void connect(std::string adress, unsigned int port = 3000);
	bool isConnected();
	
	void setConnectCallback(std::function<void()> callback);
	void setDisconnectCallback(std::function<void()> callback);
	void setInitStateCallback(std::function<void(Configuration, Gamestate *)> callback);

	
	void update();

private:
	sio::client sioClient;
	std::shared_ptr<sio::socket> sioSocket;
	unsigned reconnectAttempts = 3;
	
	std::mutex queueLock;
	std::queue<std::function<void()>> eventQueue;

	//Callbacks
	std::function<void()> connectCallback;
	std::function<void()> disconnectCallback;
	std::function<void(Configuration, Gamestate*)> initStateCallback;

	void onStateReceive(sio::event event);
	void onInitStateReceive(sio::event event);

	Gamestate* parseGamestate(std::string jsonString);
};
