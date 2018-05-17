#include <iostream>
#include <cstdint>

#include <gamestate/Map.h>
#include <gamestate/Gamestate.h>
#include <input/Input.h>
#include <renderer/Renderer.h>
#include <imgui/imgui.h>
#include <renderer/Sprites.h>

Map::Map(Input& _input, const Configuration& _config)
	: input{ _input }
	, config{_config}
	, gamestate{nullptr}
	, sprites{tex}
{

}

Map::~Map()
{
	delete this->gamestate;
}

void Map::updateGameState(class Gamestate* newState)
{
	delete this->gamestate;
	this->gamestate = newState;
}

void Map::draw(class Renderer& renderer)
{
	ImGui::Text("Connected!");
}

