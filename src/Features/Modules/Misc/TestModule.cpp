//
// Created by vastrakai on 6/28/2024.
//

#include "TestModule.hpp"

#include <imgui.h>
#include <magic_enum.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/Inventory/PlayerInventory.hpp>
#include <SDK/Minecraft/Network/MinecraftPackets.hpp>
#include <SDK/Minecraft/Network/Packets/InventoryTransactionPacket.hpp>
#include <SDK/Minecraft/World/Block.hpp>
#include <SDK/Minecraft/World/BlockLegacy.hpp>
#include <SDK/Minecraft/World/BlockSource.hpp>
#include <SDK/Minecraft/World/Level.hpp>
#include <spdlog/spdlog.h>
#include <Utils/MiscUtils/SoundUtils.hpp>

void TestModule::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &TestModule::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->listen<RenderEvent, &TestModule::onRenderEvent>(this);
}

void TestModule::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &TestModule::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->deafen<RenderEvent, &TestModule::onRenderEvent>(this);
}

Block* gDaBlock = nullptr;

void TestModule::onBaseTickEvent(BaseTickEvent& event)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    //player->jumpFromGround();
    gDaBlock = ClientInstance::get()->getBlockSource()->getBlock(glm::ivec3(0, 0, 0));

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

    if (gDaBlock)
    {
        displayCopyableAddress("fuckyou", gDaBlock->mLegacy->mVfTable[85]);
    }

    displayCopyableAddress("swing", player->vtable[117]);

    ImGui::Text("isOnGround: %d", player->isOnGround());
    ImGui::Text("wasOnGround: %d", player->wasOnGround());
    ImGui::Text("isInWater: %d", player->isInWater());
    displayCopyableAddress("getHitResult", player->getLevel()->mVfTable[288]);

    ImGui::End();
}