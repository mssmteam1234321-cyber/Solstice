//
// Created by vastrakai on 6/28/2024.
//

#include "TestModule.hpp"

#include <spdlog/spdlog.h>
#include <Utils/ChatUtils.hpp>

void TestModule::onEnable()
{
    ChatUtils::displayClientMessage("§6TestModule", "enabled!");
}

void TestModule::onDisable()
{
    ChatUtils::displayClientMessage("§6TestModule", "disabled!");
}
