//
// Created by vastrakai on 6/28/2024.
//

#include "TestModule.hpp"

#include <imgui.h>
#include <Features/Events/BaseTickEvent.hpp>
#include <Features/Events/RenderEvent.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Rendering/GuiData.hpp>
#include <spdlog/spdlog.h>
#include <Utils/GameUtils/ChatUtils.hpp>

void TestModule::onEnable()
{
    ClientInstance* client = ClientInstance::get();
    GuiData* guiData = client->getGuiData();
    ChatUtils::displayClientMessage("§6TestModule", "Screen size: " + std::to_string(guiData->resolution.x) + "x" + std::to_string(guiData->resolution.y));
    ChatUtils::displayClientMessage("§6TestModule", "Screen size Rounded: " + std::to_string(guiData->resolutionRounded.x) + "x" + std::to_string(guiData->resolutionRounded.y));
    ChatUtils::displayClientMessage("§6TestModule", "Screen size Scaled: " + std::to_string(guiData->resolutionScaled.x) + "x" + std::to_string(guiData->resolutionScaled.y));

    gFeatureManager->mDispatcher->listen<RenderEvent, &TestModule::onRenderEvent>(this);
}

void TestModule::onDisable()
{
    gFeatureManager->mDispatcher->deafen<RenderEvent, &TestModule::onRenderEvent>(this);
    ChatUtils::displayClientMessage("§6TestModule", "disabled!");
}

void TestModule::onBaseTickEvent(BaseTickEvent& event)
{
    spdlog::info("TestModule base tick event");
}

void TestModule::onRenderEvent(RenderEvent& event)
{
    // render imgui test
    ImGui::Begin("TestModule");
    ImGui::Text("TestModule");
    ImGui::End();
}