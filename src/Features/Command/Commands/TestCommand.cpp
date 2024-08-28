//
// Created by vastrakai on 6/28/2024.
//

#include "TestCommand.hpp"

#include <Utils/GameUtils/ChatUtils.hpp>
#include <magic_enum.hpp>
#include <Features/IRC/WorkingVM.hpp>
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
        /*string sample = "Hello, World!";
        string key = "Eh/fS4Q90JlI2ERc";
        string encrypted = EncUtils.Encrypt(sample, key);
        string decrypted = EncUtils.Decrypt(encrypted, key);
        Console.WriteLine("Original: " + sample);
        Console.WriteLine("Encrypted: " + encrypted);
        Console.WriteLine("Decrypted: " + decrypted);
        Console.WriteLine("Key: " + key);
        */
        std::string proofTask = "zc/Pz8/Pzs/OvM3Pz8/Pz87Px8zNz8/Pz8/Nz7vMzs/Pz8/Pz8/NvsvPz8/Pz8/Pz88=";
        int result = WorkingVM::SolveProofTask(proofTask);
        ChatUtils::displayClientMessage("Proof task result: " + std::to_string(result));
        ChatUtils::displayClientMessage("Proof task string: " + proofTask);
        ChatUtils::displayClientMessage("Expected result: 1952345");
        ChatUtils::displayClientMessage("Matches: " + std::to_string(result == 1952345));
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
