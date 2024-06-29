//
// Created by vastrakai on 6/28/2024.
//

#include "CommandManager.hpp"

#include <sstream>
#include <Utils/ChatUtils.hpp>
#include <Utils/StringUtils.hpp>

#include "spdlog/spdlog.h"

void CommandManager::init()
{
    for (auto& future : mCommandFutures)
    {
        future.get();
    }
}

void CommandManager::handleCommand(const std::string& command, bool* cancel)
{
    // If the string doesn't start with a dot, it's not a command
    if (command[0] != '.')
        return;

    *cancel = true;

    // Remove the dot from the command
    std::string cmd = command.substr(1);

    // Split the command into arguments
    const std::vector<std::string> args = StringUtils::split(cmd, ' ');

    if (args.empty())
        return;

    // Find the command
    const std::string_view commandName = args[0];

    auto it = std::ranges::find_if(mCommands, [=](const auto& command)
    {
        return command->matchName(commandName);
    });

    if (it == mCommands.end())
    {
        ChatUtils::displayClientMessage("§cThe command §6'" + std::string(commandName) + "'§c does not exist!");
        return;
    }

    // Execute the command
    (*it)->execute(args);
}
