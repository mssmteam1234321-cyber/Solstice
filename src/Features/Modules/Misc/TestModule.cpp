//
// Created by vastrakai on 6/28/2024.
//

#include "TestModule.hpp"

#include <Features/Events/PacketInEvent.hpp>
#include <Features/Events/PacketOutEvent.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/Network/MinecraftPackets.hpp>
#include <SDK/Minecraft/Network/Packets/PlayerAuthInputPacket.hpp>
#include <SDK/Minecraft/World/Block.hpp>
#include <SDK/Minecraft/World/BlockSource.hpp>
#include <SDK/Minecraft/World/Level.hpp>

void TestModule::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &TestModule::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->listen<RenderEvent, &TestModule::onRenderEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketInEvent, &TestModule::onPacketInEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketOutEvent, &TestModule::onPacketOutEvent>(this);
}

void TestModule::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &TestModule::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->deafen<RenderEvent, &TestModule::onRenderEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketInEvent, &TestModule::onPacketInEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketOutEvent, &TestModule::onPacketOutEvent>(this);
}

Block* gDaBlock = nullptr;

void TestModule::onBaseTickEvent(BaseTickEvent& event)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;


    std::unordered_map<mce::UUID, PlayerListEntry>* playerList = player->getLevel()->getPlayerList();
    std::vector<std::string> playerNames;
    for (auto& [uuid, entry] : *playerList)
    {
        playerNames.emplace_back(entry.name);
    }
    static std::vector<std::string> lastPlayerNames = playerNames;

    // Check if the player list has changed
    // Use ChatUtils::displayClientMessageRaw to display a message in the chat, depending on the player list
    // example: "§a§l» §r§7player has joined." or "§c§l» §r§7player has left."

    for (auto& playerName : playerNames)
    {
        if (std::ranges::find(lastPlayerNames, playerName) == lastPlayerNames.end())
        {
            ChatUtils::displayClientMessageRaw("§a§l» §r§7" + playerName + " has joined.");
        }
    }

    for (auto& playerName : lastPlayerNames)
    {
        if (std::ranges::find(playerNames, playerName) == playerNames.end())
        {
            ChatUtils::displayClientMessageRaw("§c§l» §r§7" + playerName + " has left.");
        }
    }

    // Store the last player list
    lastPlayerNames = playerNames;
}

void TestModule::onPacketOutEvent(PacketOutEvent& event)
{
    if (event.packet->getId() == PacketID::PlayerAuthInput)
    {
    }
}

void TestModule::onPacketInEvent(PacketInEvent& event)
{
    if (event.mPacket->getId() == PacketID::PlayerList)
    {

    }
}

void displayCopyableAddress(const std::string& name, void* address)
{
    std::string addressHex = "0x" + fmt::format("{:X}", reinterpret_cast<uintptr_t>(address));
    ImGui::Text(addressHex.c_str());
    ImGui::SameLine();
    if (ImGui::Button(("Copy " + name + " address").c_str()))
    {
        ImGui::SetClipboardText(addressHex.c_str());
    }


}

void TestModule::onRenderEvent(RenderEvent& event)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    ImGui::Begin("TestModule");
    ImGui::Text("TestModule");
    auto blockSource = ClientInstance::get()->getBlockSource();

    ImGui::Text("isOnGround: %d", player->isOnGround());
    ImGui::Text("wasOnGround: %d", player->wasOnGround());
    ImGui::Text("isInWater: %d", player->isInWater());
    displayCopyableAddress("getPlayerList", player->getLevel()->mVfTable[273]);

    ImGui::End();
}