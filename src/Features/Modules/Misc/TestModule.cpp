//
// Created by vastrakai on 6/28/2024.
//

#include "TestModule.hpp"

#include <imgui.h>
#include <Features/Events/BaseTickEvent.hpp>
#include <Features/Events/RenderEvent.hpp>
#include <spdlog/spdlog.h>
#include <Utils/GameUtils/ChatUtils.hpp>

void TestModule::onEnable()
{
    ChatUtils::displayClientMessage("§6TestModule", "enabled!");
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &TestModule::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->listen<RenderEvent, &TestModule::onRenderEvent>(this);
}

void TestModule::onDisable()
{
    ChatUtils::displayClientMessage("§6TestModule", "disabled!");
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &TestModule::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->deafen<RenderEvent, &TestModule::onRenderEvent>(this);
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