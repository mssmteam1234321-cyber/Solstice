//
// Created by vastrakai on 6/28/2024.
//
#pragma once

enum struct EventPriorities {
    ABSOLUTE_FIRST = 0,
    VERY_FIRST = 1,
    FIRST = 2,
    NORMAL = 3,
    LAST = 4,
    VERY_LAST = 5,
    ABSOLUTE_LAST = 6
};
#define NES_PRIORITY_TYPE EventPriorities

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