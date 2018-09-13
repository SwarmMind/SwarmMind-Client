#include <gamestate/TutorialNetworker.h>
#include <events/EventSystem.h>
#include <events/InitStateEvent.h>
#include <events/ChatEvent.h>
#include <renderer/Renderer.h>
#include <game/Camera.h>

TutorialNetworker::TutorialNetworker(class EventSystem& eventSystem)
    : m_eventSystem{eventSystem}
{}

void TutorialNetworker::sendCommand(uint32_t unitID, std::string action, glm::vec2 direction)
{
    AccumulatedCommands commands;
    commands.ID = unitID;
    if (action == "move")
    {
        commands.moveDirections = { direction };
    }
    else
    {
        commands.attackDirections = { direction };
    }

    AccumulatedCommandsEvent commandsEvent;
    commandsEvent.commands = { commands };
    commandsEvent.maxNumberOfCommands = 1;
    commandsEvent.numberOfGivenCommands = 0;

    m_eventSystem.processEvent(&commandsEvent);
}

void TutorialNetworker::sendChatMessage(struct ChatEntry& chatEntry)
{
    ChatEvent chatEvent;
    chatEvent.m_chatEntry = chatEntry;
    m_eventSystem.processEvent(&chatEvent);
}

void TutorialNetworker::update(double deltaTime, double timeStamp)
{
    if (m_messages.empty())
    {
        return;
    }


    m_delayToNextMessage -= deltaTime;
    while(m_delayToNextMessage <= 0)
    {
        TutorialMessage message = m_messages.front();
        m_messages.pop(); //pop removes the last element, as m_messages is a queue
        m_messages.push(message);
        m_delayToNextMessage += m_messages.front().m_delay;

        ChatEntry chatEntry;
        chatEntry.m_position = message.m_position;
        chatEntry.m_text = message.m_message;
        chatEntry.m_timeStamp = timeStamp;
        chatEntry.m_user = "Tutorial";
        
        ChatEvent chatEvent;
        chatEvent.m_chatEntry = chatEntry;
        m_eventSystem.processEvent(&chatEvent);
    }
}

void TutorialNetworker::begin(Renderer& renderer)
{
    InitStateEvent startEvent;
    Configuration config;
    config.m_roundTime = m_roundTime;
    config.m_sizeX = m_mapSize;
    config.m_sizeY = m_mapSize;
    startEvent.m_config = config;

    m_unitPosition = glm::vec2(m_mapSize / 2, m_mapSize / 2) + 0.5f;
    std::map<uint32_t, Unit> units;
    units.emplace(m_unitID, Unit(m_unitID, m_unitPosition));

    units.emplace(m_unitID + 1, Unit(m_unitID + 1, m_unitPosition - glm::vec2(1.0, 0.0)));
    units.emplace(m_unitID + 2, Unit(m_unitID + 2, m_unitPosition - glm::vec2(1.0, -1.0)));
    
    m_monsterPosition = glm::vec2(m_unitPosition + glm::vec2(2.0, 2.0));
    std::map<uint32_t, Monster> monsters;
    uint32_t monsterID = m_unitID + 3;
    monsters.emplace(monsterID, Monster(monsterID, m_monsterPosition));

    startEvent.m_state = new Gamestate(m_eventSystem, units, monsters);

    m_eventSystem.processEvent(&startEvent);


    createMessages();
    m_delayToNextMessage = m_messages.front().m_delay;

    renderer.camera().setCamera(14, 11, 7);
}

void TutorialNetworker::createMessages()
{
    glm::vec2 defaultPosition(12.0, 10.0);
    m_messages.emplace("Welcome to SwarmMind!", defaultPosition, 2.0);
    
    m_messages.emplace(
"Let us first show you how to control the camera.\n\
To move the camera, either use middle-click, or the arrow keys!\n\
To zoom in and out use your scroll wheel!", defaultPosition);
    m_messages.emplace("I will give you a few seconds, so you can try that out.", defaultPosition);
    
    //Units & Giving commands
    m_messages.emplace("Now let's move on to more interesting topics ;)", defaultPosition - 1.0f, 20.f);
    m_messages.emplace("This is one of your Units.", m_unitPosition);
    m_messages.emplace(
"You can give it commands the following ways:\n\
Move  - Drag in the direction you want to move\n\
Shoot - Right-click drag in the direction you want to shoot\n\
You can try it now, if you want to.", m_unitPosition);
    
    //Rounds and other players
    m_messages.emplace(
"Whenever a round ends, the commands will all be executed at the same time.\n\
There are two ways a round can end:\n\
   1. The time is up\n\
   2. Enough commands have been issued\n", m_unitPosition);
    m_messages.emplace(
"The progress bars at the bottom will show when the round ends.\n\
We have disabled this for the Tutorial.", m_unitPosition);
    m_messages.emplace(
"Beware however that you will not be the only one issuing commands!\n\
Any other player that is connected to the same server can also give commands to your Units!\n", m_unitPosition);
    m_messages.emplace(
"This means that you will have to talk with the other players to make sure you\n\
don't get in conflict with the other players.", m_unitPosition);


    //Chat system
    m_messages.emplace(
"To place a chat message on the map, double click anywhere.\n\
Try it now!", defaultPosition);
    m_messages.emplace("You can freely change your username in the top right corner of your screen.", defaultPosition, 10.0);

    //Enemies
    m_messages.emplace("So, why are we doing all this command issuing and chatting again?", defaultPosition + glm::vec2(1.0), 15.f);
    m_messages.emplace("This is an enemy!", m_monsterPosition);
    m_messages.emplace(
"Enemies will move toward your units and try to kill them at the end of the round.\n\
Your job is to protect your units from the incoming enemies and to survive as long as possible!", m_monsterPosition);

    //Join a server
    m_messages.emplace(
"Now you are ready to join a server!\n\
Use the window at the top to enter the servers adress and connect!\n\
Have fun playing", defaultPosition);

    //To not immediately start the Tutorial again
    m_messages.emplace("Congratulations! You found this hidden message :D", glm::vec2(-20000, -20000), 10.0);

}

