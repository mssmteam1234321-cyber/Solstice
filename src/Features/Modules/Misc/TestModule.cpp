//
// Created by vastrakai on 6/28/2024.
//

#include "TestModule.hpp"

#include <imgui.h>
#include <Features/Events/BaseTickEvent.hpp>
#include <Features/Events/RenderEvent.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Rendering/GuiData.hpp>
#include <SDK/Minecraft/World/Block.hpp>
#include <SDK/Minecraft/World/BlockLegacy.hpp>
#include <SDK/Minecraft/World/BlockSource.hpp>
#include <spdlog/spdlog.h>
#include <Utils/GameUtils/ChatUtils.hpp>
#include <Utils/GameUtils/PacketUtils.hpp>
#include <Utils/MiscUtils/SoundUtils.hpp>

void TestModule::onEnable()
{
    gFeatureManager->mDispatcher->listen<RenderEvent, &TestModule::onRenderEvent>(this);
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &TestModule::onBaseTickEvent>(this);
}

void TestModule::onDisable()
{

    gFeatureManager->mDispatcher->deafen<RenderEvent, &TestModule::onRenderEvent>(this);
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &TestModule::onBaseTickEvent>(this);
    ChatUtils::displayClientMessage("§6TestModule", "disabled!");
}

Block* gDaBlock = nullptr;

void TestModule::onBaseTickEvent(BaseTickEvent& event)
{
    gDaBlock = ClientInstance::get()->getBlockSource()->getBlock(0, 0, 0);
    PacketUtils::spoofSlot(0);
}

void displayCopyableAddress(std::string name, void* address)
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
    ImGui::Begin("TestModule");
    ImGui::Text("TestModule");
    auto blockSource = ClientInstance::get()->getBlockSource();
    auto block = gDaBlock;
    if (block == nullptr)
    {
        ImGui::Text("Block is null");
        ImGui::End();
        return;
    }

    displayCopyableAddress("BlockSource", blockSource);
    displayCopyableAddress("Block", block);
    displayCopyableAddress("BlockLegacy", block->mLegacy);

    ImGui::Button("Play Sound Test");
    if (ImGui::IsItemClicked())
    {
        SoundUtils::playSoundFromEmbeddedResource("fard.wav", 1.0f);
    }

    ImGui::End();
}