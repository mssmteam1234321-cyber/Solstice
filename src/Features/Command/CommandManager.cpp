//
// Created by vastrakai on 6/28/2024.
//

#include "CommandManager.hpp"

#include <sstream>
#include <Features/FeatureManager.hpp>
#include <Utils/GameUtils/ChatUtils.hpp>
#include <Utils/StringUtils.hpp>

#include "Commands/BindCommand.hpp"
#include "Commands/BuildInfoCommand.hpp"
#include "Commands/ConfigCommand.hpp"
#include "Commands/FriendCommand.hpp"
#include "Commands/HelpCommand.hpp"
#include "Commands/QueueCommand.hpp"
#include "Commands/SetCommand.hpp"
#include "Commands/TestCommand.hpp"
#include "Commands/ToggleCommand.hpp"
#include "Commands/UnbindCommand.hpp"
#include "Commands/ModuleCommand.hpp"
#include "Commands/GamemodeCommand.hpp"
#include "Commands/HiveStatsCommand.hpp"
#include "Features/Events/ChatEvent.hpp"
#include "spdlog/spdlog.h"

void CommandManager::init()
{
    ADD_COMMAND(HelpCommand);
    ADD_COMMAND(TestCommand);
    ADD_COMMAND(ToggleCommand);
    ADD_COMMAND(BindCommand);
    ADD_COMMAND(UnbindCommand);
    ADD_COMMAND(ConfigCommand);
    ADD_COMMAND(QueueCommand);
    ADD_COMMAND(SetCommand);
    ADD_COMMAND(ModuleCommand);
    ADD_COMMAND(FriendCommand);
    ADD_COMMAND(GamemodeCommand);
    ADD_COMMAND(BuildInfoCommand);
    ADD_COMMAND(HiveStatsCommand);

    gFeatureManager->mDispatcher->listen<ChatEvent, &CommandManager::handleCommand>(this);
}

void CommandManager::shutdown()
{
    gFeatureManager->mDispatcher->deafen<ChatEvent, &CommandManager::handleCommand>(this);
    mCommands.clear();

    spdlog::info("Successfully shut down CommandManager");
}

void CommandManager::handleCommand(ChatEvent& event)
{
    std::string command = event.getMessage();

    if (command[0] != '.')
        return;

    event.setCancelled(true);

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
    spdlog::info("Executing command: {}", commandName);
    (*it)->execute(args);
}

std::vector<Command*> CommandManager::getCommands() const
{
    std::vector<Command*> commands;
    commands.reserve(mCommands.size());

    for (const auto& command : mCommands)
    {
        commands.push_back(command.get());
    }

    return commands;
}
