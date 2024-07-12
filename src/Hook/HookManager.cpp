//
// Created by vastrakai on 6/25/2024.
//

#include "HookManager.hpp"

#include <MinHook.h>
#include <Solstice.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <Utils/ProcUtils.hpp>

void HookManager::init()
{
    // Wait for mFutures
    for (auto& future : mFutures)
    {
        future.wait();
    }
}

void HookManager::shutdown()
{
    for (auto& hook : mHooks)
    {
        hook->shutdown();
    }

    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();

    mHooks.clear();
    mFutures.clear();
    mFutures2.clear();
}
