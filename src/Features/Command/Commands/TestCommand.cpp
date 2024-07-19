//
// Created by vastrakai on 6/28/2024.
//

#include "TestCommand.hpp"

#include <Utils/GameUtils/ChatUtils.hpp>
#include <magic_enum.hpp>

void TestCommand::execute(const std::vector<std::string>& args)
{
    ChatUtils::displayClientMessage("Test command executed!");
    // Display the amount of Modules and Commands
    ChatUtils::displayClientMessage(std::to_string(gFeatureManager->mModuleManager->mModules.size()) + " modules and "
        + std::to_string(gFeatureManager->mCommandManager->mCommands.size()) + " commands are currently loaded.");

    // get the first argument, if any
    if (args.size() > 1)
    {
        const std::string& arg = args[1];
        if (arg == "showconsole")
        {
            Logger::initialize();
            ChatUtils::displayClientMessage("Console initialized!");
            return;
        }

        // spdlog::level::trace
        for (std::string_view level : magic_enum::enum_names<spdlog::level::level_enum>())
        {
            spdlog::info("Checking level {}", level);
            if (level == arg)
            {
                spdlog::info("Setting log level to {}", level);
                spdlog::set_level(spdlog::level::from_str(level.data()));
                ChatUtils::displayClientMessage("Log level set to " + std::string(level));
                return;
            }
        }
    }
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
