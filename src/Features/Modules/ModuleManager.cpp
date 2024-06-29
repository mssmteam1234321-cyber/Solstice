//
// Created by vastrakai on 6/28/2024.
//

#include "ModuleManager.hpp"

void ModuleManager::init()
{
    for (auto& future : mModuleFutures)
    {
        future.get();
    }

    for (auto& module : mModules)
    {
        if (module->mEnabled)
            module->onEnable();

    }
}

void ModuleManager::shutdown()
{
    for (auto& module : mModules)
    {
        if (module->mEnabled)
        {
            module->mEnabled = false;
            module->onDisable();
        }
    }

    mModules.clear();
    mModuleFutures.clear();
}

void ModuleManager::registerModule(std::unique_ptr<Module> module)
{
    mModules.push_back(std::move(module));
}

std::vector<std::unique_ptr<Module>>& ModuleManager::getModules()
{
    return mModules;
}
