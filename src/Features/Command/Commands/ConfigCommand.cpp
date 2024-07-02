//
// Created by vastrakai on 7/2/2024.
//

#include "ConfigCommand.hpp"

#include <Features/Configs/ConfigManager.hpp>
#include <Utils/FileUtils.hpp>
#include <Utils/GameUtils/ChatUtils.hpp>

void ConfigCommand::execute(const std::vector<std::string>& args)
{
    if (args.size() < 2)
    {
        ChatUtils::displayClientMessage("§c" + getUsage());
        return;
    }

    std::string action = args[1];

    if (action == "load" || action == "l")
    {
        if (args.size() < 3)
        {
            ChatUtils::displayClientMessage("§c" + getUsage());
            return;
        }

        const std::string name = args[2];

        if (!ConfigManager::configExists(name))
        {
            ChatUtils::displayClientMessage("§cThe config §6'" + name + "' §cdoes not exist.");
            return;
        }

        ConfigManager::loadConfig(name);
        ChatUtils::displayClientMessage("§aLoaded config §6" + name + "§a!");
    }
    else if (action == "save" || action == "s")
    {
        if (args.size() < 3)
        {
            ChatUtils::displayClientMessage("§c" + getUsage());
            return;
        }

        std::string name = args[2];
        ConfigManager::saveConfig(name);
        ChatUtils::displayClientMessage("§aSaved config as §6" + name + "§a.");
    }
    else if (action == "list" || action == "l")
    {
        ChatUtils::displayClientMessage("§aConfigs:");
        for (const auto& f : FileUtils::listFiles(ConfigManager::getConfigPath()))
        {
            std::string file = f;
            // Remove the .json from the end of the file if it exists
            if (file.ends_with(".json"))
            {
                file = file.substr(0, file.size() - 5);
            } else continue;
            ChatUtils::displayClientMessage("§7- " + file);
        }

        ChatUtils::displayClientMessage("§aType §6.config load <name> §ato load a config.");
    }
    else if (action == "delete" || action == "del")
    {
        if (args.size() < 3)
        {
            ChatUtils::displayClientMessage("§c" + getUsage());
            return;
        }

        const std::string& name = args[2];

        if (!ConfigManager::configExists(name))
        {
            ChatUtils::displayClientMessage("§cThe config §6'" + name + "' §cdoes not exist.");
            return;
        }

        if (!FileUtils::deleteFile(ConfigManager::getConfigPath() + name + ".json"))
        {
            ChatUtils::displayClientMessage("§cUnable to delete config!");
            return;
        }

        ChatUtils::displayClientMessage("§aSuccessfully deleted config §6" + name + "§a.");
    }
    else
    {
        ChatUtils::displayClientMessage("§c" + getUsage());
    }

}

std::vector<std::string> ConfigCommand::getAliases() const
{
    return {"c"};
}

std::string ConfigCommand::getDescription() const
{
    return "Manage configs.";
}

std::string ConfigCommand::getUsage() const
{
    return "Usage: .config <load/save/list/delete> <name>";
}
