//
// Created by vastrakai on 9/21/2024.
//

#include "CustomChat.hpp"

#include <Features/Events/ChatEvent.hpp>
#include <Features/Events/PacketInEvent.hpp>
#include <SDK/Minecraft/Network/Packets/TextPacket.hpp>

void CustomChat::onEnable()
{
    gFeatureManager->mDispatcher->listen<PacketInEvent, &CustomChat::onPacketInEvent, nes::event_priority::ABSOLUTE_LAST>(this);
    gFeatureManager->mDispatcher->listen<RenderEvent, &CustomChat::onRenderEvent>(this);
}

void CustomChat::onDisable()
{
    gFeatureManager->mDispatcher->deafen<PacketInEvent, &CustomChat::onPacketInEvent>(this);
    gFeatureManager->mDispatcher->deafen<RenderEvent, &CustomChat::onRenderEvent>(this);
}

// Minecraft: Java edition styled chat
void CustomChat::onRenderEvent(RenderEvent& event)
{
    FontHelper::pushPrefFont();

    auto drawList = ImGui::GetBackgroundDrawList();

    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    ImVec2 windowSize = {300, 200};
    ImVec2 windowPos = {10, 10};

    drawList->AddRectFilled(windowPos, {windowPos.x + windowSize.x, windowPos.y + windowSize.y}, IM_COL32(0, 0, 0, 200));

    float fontSize = 20;

    ImVec2 cursorPos = {windowPos.x + 10, windowPos.y + 10};



    for (auto& message : mMessages)
    {
        ImVec2 textPos = cursorPos;

        drawList->AddText(ImGui::GetFont(), fontSize, cursorPos, IM_COL32(255, 255, 255, 255), message.c_str());
        cursorPos.y += ImGui::GetFont()->CalcTextSizeA(fontSize, FLT_MAX, 0, message.c_str()).y + 5;
    }

    FontHelper::popPrefFont();
}

void CustomChat::onPacketInEvent(PacketInEvent& event)
{
    if (event.isCancelled()) return;
    if (event.mPacket->getId() != PacketID::Text) return;

    auto textPacket = event.getPacket<TextPacket>();
    std::string message = textPacket->mMessage;
    if (textPacket->mType == TextPacketType::Chat) {
        message = "<" + textPacket->mAuthor + "> " + message;
    }

    mMessages.push_back(message);
}