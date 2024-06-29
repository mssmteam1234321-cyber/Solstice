//
// Created by vastrakai on 6/29/2024.
//

#include "HelpCommand.hpp"

#include <Utils/ChatUtils.hpp>

void HelpCommand::execute(const std::vector<std::string>& args)
{
    const auto commands = gFeatureManager->mCommandManager->getCommands();

    if (args.size() > 1)
    {
        for (const auto& command : commands)
        {
            if (command->name == args[1])
            {
                ChatUtils::displayClientMessage("§6Command: §e" + command->name + " §7- " + command->getUsage());
                return;
            }
        }

        ChatUtils::displayClientMessage("§cThe command §6'" + args[1] + "'§c does not exist!");
        return;
    }

    ChatUtils::displayClientMessage("§6Commands:");

    for (const auto& command : commands)
    {
        ChatUtils::displayClientMessage("§6- §e" + command->name + "§7: " + command->getDescription());
    }

    ChatUtils::displayClientMessage("§6Use §e.help <command> §6for more information on a specific command.");
}

std::vector<std::string> HelpCommand::getAliases() const
{
    return {};
}

std::string HelpCommand::getDescription() const
{
    return "The help command";
}

std::string HelpCommand::getUsage() const
{
    return "Usage: .help";
}
