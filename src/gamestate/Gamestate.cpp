#include <gamestate/Gamestate.h>

Gamestate::Gamestate(Networker& _networker)
	: networker{_networker}
	, gameState{nullptr}
{

}

Gamestate::~Gamestate()
{
	delete this->gameState;
}

void Gamestate::update(double deltaTime)
{
	if (stateConfiguration.empty())
	{
		this->updateConfiguration();
	}
	else
	{
		this->updateGameState();
	}
}

void Gamestate::updateConfiguration()
{
	if (networker.isConnected())
	{
		stateConfiguration = networker.getConfiguration();
		if (!stateConfiguration.empty())
		{
			this->updateGameState();
		}
	}
}

void Gamestate::updateGameState()
{
	if (networker.isConnected())
	{
		nlohmann::json* newState = networker.getCurrentState();
		if (newState)
		{
			delete this->gameState;
			this->gameState = newState;
		}
	}
}

void Gamestate::drawConnecting()
{

}

void Gamestate::draw(Renderer& renderer)
{

}
