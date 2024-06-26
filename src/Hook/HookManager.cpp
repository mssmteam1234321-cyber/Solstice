//
// Created by vastrakai on 6/25/2024.
//

#include "HookManager.hpp"

void HookManager::init()
{
    // Wait for mFutures
    for (auto& future : mFutures)
    {
        future.wait();
    }
}
