//
// Created by vastrakai on 6/28/2024.
//

#include "TestModule.hpp"

#include <spdlog/spdlog.h>

void TestModule::onEnable()
{
    spdlog::info("TestModule enabled!");
}

void TestModule::onDisable()
{
    spdlog::info("TestModule disabled!");
}
