//
// Created by vastrakai on 6/25/2024.
//

#include "HookManager.hpp"

#include <Solstice.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>

void HookManager::init()
{
    // Wait for mFutures
    for (auto& future : mFutures)
    {
        future.wait();
    }
}
