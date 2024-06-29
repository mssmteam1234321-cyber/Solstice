#pragma once
#include <future>
#include <memory>
#include <vector>

#include "Command.hpp"
//
// Created by vastrakai on 6/28/2024.
//


class CommandManager {
public:
    std::vector<std::unique_ptr<Command>> mCommands;
    static inline std::vector<std::future<void>> mCommandFutures;

    void init();
    void handleCommand(const std::string& command, bool* cancel);
};

// Macro for command registration
#define REGISTER_COMMAND(COMMAND_CLASS) \
    namespace { \
        struct COMMAND_CLASS ## _Registrator { \
            COMMAND_CLASS ## _Registrator() { \
                static bool isRegistered = false; \
                if (isRegistered) return; \
                isRegistered = true; \
                CommandManager::mCommandFutures.push_back(std::async(std::launch::async, []() { \
                    while (!gFeatureManager || !gFeatureManager->mCommandManager) { \
                        if (Solstice::mRequestEject) return; \
                        std::this_thread::sleep_for(std::chrono::milliseconds(1)); \
                    } \
                    if (Solstice::mRequestEject) return; \
                    gFeatureManager->mCommandManager->mCommands.push_back(std::make_unique<COMMAND_CLASS>()); \
                    spdlog::info("Registered command: {}", #COMMAND_CLASS); \
                })); \
            } \
        }; \
        static COMMAND_CLASS ## _Registrator COMMAND_CLASS ## _registrator; \
    }