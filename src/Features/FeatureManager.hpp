//
// Created by vastrakai on 6/28/2024.
//
#pragma once

#include <nes/event_dispatcher.hpp>
#include "Command/CommandManager.hpp"
#include "Modules/ModuleManager.hpp"


class FeatureManager {
public:
    std::unique_ptr<nes::event_dispatcher> mDispatcher;
    std::unique_ptr<ModuleManager> mModuleManager = nullptr;
    std::unique_ptr<CommandManager> mCommandManager = nullptr;

    void init();
    void shutdown();
};

inline std::unique_ptr<FeatureManager> gFeatureManager = nullptr;