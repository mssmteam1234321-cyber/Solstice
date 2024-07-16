#pragma once
#include <Hook/Hook.hpp>
#include <Hook/HookManager.hpp>
#include <SDK/OffsetProvider.hpp>
#include <SDK/SigManager.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <Solstice.hpp>

#include "Hook.hpp"
//
// Created by vastrakai on 6/25/2024.
//

class HookManager {
public:
    static inline std::vector<std::shared_ptr<Hook>> mHooks;


    static void init(bool initLp);
    static void shutdown();
    static void waitForHooks();
};