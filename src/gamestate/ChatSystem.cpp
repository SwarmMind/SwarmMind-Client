#include <gamestate/ChatSystem.h>
#include <imgui/imgui.h>
#include <renderer/Renderer.h>
#include <glm/glm.hpp>
#include <gamestate/Networker.h>
#include <renderer/Sprites.h>
#include <cstring>

ChatSystem::ChatSystem(Input& input, Networker& networker, EventSystem& eventSystem, const std::string& preset_username)
    : EventListener<ChatEvent>{ eventSystem }
    , m_input{input}
    , m_networker{ networker } {
	strcpy(m_userName, preset_username.c_str());
}

void ChatSystem::update(double deltaTime, double timeStamp)
{
    m_lastTimeStamp = timeStamp;
    drawPopup(timeStamp);

    while (!m_chats.empty() && timeStamp - m_chats.front().m_timeStamp > m_chatVisibilityTimeout)
    {
        m_chats.pop_front();
    }


    if (m_doubleClickTimeout >= 0)
    {
        m_doubleClickTimeout -= deltaTime;
    }

    if (m_input.isActionJustPressed(SelectUnit))
    {
        if (m_doubleClickTimeout >= 0)
        {
            m_clickPosition = m_input.mousePositionInWorld();
            openPopup();
        }
        else
        {
            m_doubleClickTimeout = m_doubleClickThreshold;
        }
    }
}

void ChatSystem::openPopup()
{
    //Reset chat input
    m_chatInput[0] = '\0';
    ImGui::OpenPopup("ChatInputPopup");
}

void ChatSystem::drawPopup(double timeStamp)
{
    ImGui::SetNextWindowFocus();
    if (ImGui::BeginPopup("ChatInputPopup",
        ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoSavedSettings
        | ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Chat:");
        
        if (ImGui::IsWindowAppearing())
        {
            ImGui::SetKeyboardFocusHere();
        }
        if (ImGui::InputText("##ChatInput", m_chatInput, m_chatInputBufferSize, ImGuiInputTextFlags_EnterReturnsTrue))
        {
            buildChatEntry(timeStamp);
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::SmallButton("Ok"))
        {
            buildChatEntry(timeStamp);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void ChatSystem::buildChatEntry(double timeStamp)
{
    ChatEntry chat;
    chat.m_timeStamp = timeStamp;
    chat.m_position = m_clickPosition;
    chat.m_text = m_chatInput;
    chat.m_user = m_userName;

    //only add chat if its not empty
    if (!chat.m_text.empty())
    {
        addChat(chat);
        m_networker.sendChatMessage(chat);
    }
}

void ChatSystem::receiveEvent(ChatEvent* event)
{
    ChatEntry chat = event->m_chatEntry;
    chat.m_timeStamp = m_lastTimeStamp;
    addChat(chat);
}

void ChatSystem::addChat(ChatEntry chat)
{
    m_chats.push_back(chat);
}

void ChatSystem::draw(Renderer& renderer)
{
    ImVec2 imGuiDisplaySize = ImGui::GetIO().DisplaySize;
    glm::vec2 displaySize(imGuiDisplaySize.x, imGuiDisplaySize.y);

    float alpha = calculateAlpha(renderer.camera());

    if (alpha > 0.05)
    {
        drawChats(renderer);
    }
    else
    {
        drawChatIcons(renderer);
    }

    drawNameInput(renderer);
}

float ChatSystem::calculateAlpha(Camera &camera)
{
    return 1.0f - glm::smoothstep(8.f, 10.f, camera.height());
}

void ChatSystem::drawChatIcons(Renderer& renderer)
{
    float size = 0.5f;
    for (const ChatEntry& chat : m_chats)
    {
        glm::vec2 position = chat.m_position;
        renderer.drawSprite(glm::vec3(position.x, position.y - size, 1), size, size, SpriteEnum::ChatIcon);
    }
}

void ChatSystem::drawChats(Renderer& renderer)
{
    ImVec2 imGuiDisplaySize = ImGui::GetIO().DisplaySize;
    glm::vec2 displaySize(imGuiDisplaySize.x, imGuiDisplaySize.y);

    Camera& camera = renderer.camera();

    const float alpha = calculateAlpha(renderer.camera());

    for (size_t i = 0; i < m_chats.size(); i++)
    {
        ChatEntry& chat = m_chats[i];

        //transfer position to [-1, 1]
        glm::vec2 position = (chat.m_position - camera.position()) / camera.extent();
        //transfer position to [0, 1]
        position = (position + 1.f) / 2.f;
        //transfer to pixel coordinates
        position *= displaySize;
        //flip y
        position.y = displaySize.y - position.y;

        ImGui::SetNextWindowBgAlpha(alpha * 0.7f);
        ImGui::SetNextWindowPos(ImVec2(position.x, position.y), ImGuiCond_Always);
        if (ImGui::Begin((std::string("ChatWindow") + to_string(i)).data(),
            nullptr,
            ImGuiWindowFlags_NoTitleBar
            | ImGuiWindowFlags_AlwaysAutoResize
            | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoScrollbar
            | ImGuiWindowFlags_NoSavedSettings
            | ImGuiWindowFlags_NoInputs))
        {
            ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.2f, alpha), chat.m_user.data());
            ImGui::Separator();
            ImGui::TextColored(ImVec4(.4f, .4f, .4f, alpha), chat.m_text.data());
        }
        ImGui::End();
    }
}

void ChatSystem::drawNameInput(Renderer& renderer)
{
    ImVec2 imGuiDisplaySize = ImGui::GetIO().DisplaySize;

    ImGui::SetNextWindowPos(ImVec2(imGuiDisplaySize.x - 30.f, 30.f), ImGuiCond_Always, ImVec2(1.f, 0.f));
    ImGui::Begin("UserNameInput", nullptr
        , ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_AlwaysAutoResize);
    {
        ImGui::PushItemWidth(ImGui::GetTextLineHeight() * m_userNameBufferSize/2);
        ImGui::InputText("Username", m_userName, m_userNameBufferSize, ImGuiInputTextFlags_NoUndoRedo | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_AutoSelectAll);
    }
    ImGui::End();
}
