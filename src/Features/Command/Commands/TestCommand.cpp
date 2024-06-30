//
// Created by vastrakai on 6/28/2024.
//

#include "TestCommand.hpp"

#include <Utils/GameUtils/ChatUtils.hpp>

void TestCommand::execute(const std::vector<std::string>& args)
{
    ChatUtils::displayClientMessage("Test command executed!");
    // Display the amount of Modules and Commands
    ChatUtils::displayClientMessage(std::to_string(gFeatureManager->mModuleManager->mModules.size()) + " modules and "
        + std::to_string(gFeatureManager->mCommandManager->mCommands.size()) + " commands are currently loaded.");
}

std::vector<std::string> TestCommand::getAliases() const
{
    return {};
}

std::string TestCommand::getDescription() const
{
    return "A command for testing purposes.";
}

std::string TestCommand::getUsage() const
{
    return "Usage: .test";
}
