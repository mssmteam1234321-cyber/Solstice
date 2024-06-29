//
// Created by vastrakai on 6/28/2024.
//

#include "FeatureManager.hpp"

#include "Command/CommandManager.hpp"
#include "Modules/ModuleManager.hpp"
#include "spdlog/spdlog.h"

void FeatureManager::init()
{
    spdlog::info("initializing FeatureManager");

    spdlog::info("initializing ModuleManager");
    mModuleManager = std::make_unique<ModuleManager>();
    mModuleManager->init();

    spdlog::info("initializing CommandManager");
    mCommandManager = std::make_unique<CommandManager>();
    mCommandManager->init();
}
