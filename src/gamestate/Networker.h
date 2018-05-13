#pragma once

#include <nlohmann/json_fwd.hpp>
#include <sio_client.h>
#include <string>
#include <memory>
#include <mutex>

class Networker
{
public:
	Networker(std::string adress, unsigned int port = 3000);

	~Networker();

	bool isConnected();
	
	nlohmann::json* getCurrentState();
	nlohmann::json getConfiguration();

private:
	sio::client sioClient;
	std::shared_ptr<sio::socket> sioSocket;
	unsigned reconnectAttempts = 3;
	

	nlohmann::json* gameState;
	std::mutex gameStateMutex;

	nlohmann::json* stateConfiguration;
	std::mutex configurationMutex;

	
	void onStateReceive(sio::event event);
	void onInitStateReceive(sio::event event);

	void setState(nlohmann::json& newState);
	void setConfiguration(nlohmann::json& configuration);

	bool initialized;
};
