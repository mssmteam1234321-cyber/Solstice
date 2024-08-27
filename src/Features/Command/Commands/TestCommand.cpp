//
// Created by vastrakai on 6/28/2024.
//

#include "TestCommand.hpp"

#include <Utils/GameUtils/ChatUtils.hpp>
#include <magic_enum.hpp>
#include <Hook/Hooks/RenderHooks/D3DHook.hpp>

void TestCommand::execute(const std::vector<std::string>& args)
{
    ChatUtils::displayClientMessage("Test command executed!");
    // Display the amount of Modules and Commands
    ChatUtils::displayClientMessage(std::to_string(gFeatureManager->mModuleManager->mModules.size()) + " modules and "
        + std::to_string(gFeatureManager->mCommandManager->mCommands.size()) + " commands are currently loaded.");

    // get the first argument, if any
    if (args.size() <= 1)
    {
        ChatUtils::displayClientMessage("Available subcommands: showconsole, fallback, setloglevel[log level], enforcedebug, enctest");
        return;
    }

    const std::string& arg = args[1];

    if (arg == "enctest")
    {
        std::string input = "z8/Pz8/PzM3MzM3Pz8/Pz8rOx8/Nz8/Pz8/OzsjMzc/Pz8/PvM+5vs3Pubm5ubu7zL7Oz8/Pz8/Iz769zs+5ubm5vLq7vMzPz8/Pz8/Pu7nPz7m5ubnMusjNzc/Pz8/Pus+5uc3Pz8/Pz8fOucfPz7m5ubm5ucjJz8+5ubm5zLnPz87Pz8/Pz83Ny8/Oz7m5ubm+u8u+z8+5ubm5vLq7us7Pubm5ucm5urnPz7m5ubnLuszNzc+5ubm5x7q9zs3Pz8/Pz7rPx7zOz8/Pz8/OzcvLzs+5ubm5vLnGyM3Pubm5ucu5vrnOz8/Pz8+8zrnNzs/Pz8/Pys+7z87Pubm5ub25yL3Pz8/Pz8/Hz866z8/Pz8/Puc7Mzs7Pubm5ubu6x8/Nz8/Pz8+9zs2+zc/Pz8/Pyc3HvcvPz8/Pz8/Pz88=";
        std::string key = "UMBQ0l9YCSdxkeV+5LgD4SESbE2dk0aCBU+iYROttFM7bQJ72iRhuFV8MMcuJESQdzhbKgykC7dDiohNUWgOLA==";
        std::string output = StringUtils::decrypt(input, key);
        ChatUtils::displayClientMessage("Decrypted: " + output);
        return;
    }

    if (arg == "showconsole")
    {
        Logger::initialize();
        ChatUtils::displayClientMessage("Console initialized!");
        return;
    }

    if (arg == "fallback")
    {
        D3DHook::forceFallback = true;
        ChatUtils::displayClientMessage("Attempting to force fallback to D3D11!");
        return;
    }

    // spdlog::level::trace
    if (arg == "setloglevel")
    {
        if (args.size() < 3)
        {
            ChatUtils::displayClientMessage("Usage: .test setloglevel [log level]");
            return;
        }

        const std::string& arg2 = args[2];

        for (std::string_view level : magic_enum::enum_names<spdlog::level::level_enum>())
        {
            spdlog::info("Checking level {}", level);
            if (level == arg2)
            {
                spdlog::info("Setting log level to {}", level);
                spdlog::set_level(spdlog::level::from_str(level.data()));
                ChatUtils::displayClientMessage("Log level set to " + std::string(level));
                return;
            }
        }
        return;
    }

    if (arg == "enforcedebug")
    {
        Solstice::Prefs->mEnforceDebugging = !Solstice::Prefs->mEnforceDebugging;
        PreferenceManager::save(Solstice::Prefs);
        ChatUtils::displayClientMessage("Enforce debugging set to {}", Solstice::Prefs->mEnforceDebugging ? "true" : "false");
        ChatUtils::displayClientMessage("You may need to reinject for this to take effect.");
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
