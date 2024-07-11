//
// Created by vastrakai on 6/28/2024.
//
#pragma once

enum struct EventPriorities {
    ABSOLUTE_FIRST,
    VERY_FIRST,
    FIRST,
    KINDA_FIRST,
    NORMAL,
    KINDA_LAST,
    LAST,
    VERY_LAST,
    ABSOLUTE_LAST,
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