#include <gamestate/Map.h>
#include <input/Input.h>
#include <renderer/Renderer.h>
#include <imgui/imgui.h>

Map::Map(Input& _input) 
	: input{ _input }
	, gamestate{nullptr}
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

