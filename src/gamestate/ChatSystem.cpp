#include <gamestate/ChatSystem.h>
#include <imgui/imgui.h>
#include <renderer/Renderer.h>
#include <glm/glm.hpp>

ChatSystem::ChatSystem(Input& input, Networker& networker)
    : m_input{input}
    , m_networker{networker}
{}


void ChatSystem::update(double deltaTime, double timeStamp)
{
    drawPopup(timeStamp);

    while (m_chats.size() > 0 && timeStamp - m_chats.front().m_timeStamp > m_chatVisibilityTimeout)
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
        | ImGuiWindowFlags_NoSavedSettings))
    {
        ImGui::Text("Chat:");
        
        if (ImGui::IsWindowAppearing())
        {
            ImGui::SetKeyboardFocusHere();
        }
        if (ImGui::InputText("##ChatInput", m_chatInput, 50, ImGuiInputTextFlags_EnterReturnsTrue))
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
    chat.m_user = "this";

    //only add chat if its not empty
    if(chat.m_text.size() > 0)
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

    Camera& camera = renderer.camera();
    glm::vec2 cameraPosition = glm::vec2(camera.getX(), camera.getY());
    glm::vec2 cameraView = glm::vec2(camera.getWidth(), camera.getHeight());

    float alpha = 1.0 - glm::smoothstep(8.f, 10.f, camera.getHeight());
    if (alpha <= 0.05)
    {
        return;
    }

    for (size_t i = 0; i < m_chats.size(); i++)
    {
        ChatEntry& chat = m_chats[i];

        //transfer position to [-1, 1]
        glm::vec2 position = (chat.m_position - cameraPosition) / cameraView; 
        //transfer position to [0, 1]
        position = (position + 1.f) / 2.f;
        //transfer to pixel coordinates
        position *= displaySize;
        //flip y
        position.y = displaySize.y - position.y;

        ImGui::SetNextWindowBgAlpha(alpha * 0.7);
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
            ImGui::TextColored(ImVec4(1.0, 1.0, 0.2, alpha), chat.m_user.data());
            ImGui::Separator();
            ImGui::TextColored(ImVec4(1, 1, 1, alpha), chat.m_text.data());
        }
        ImGui::End();
    }
}
