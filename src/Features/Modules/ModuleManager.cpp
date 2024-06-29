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

void ModuleManager::registerModule(std::unique_ptr<Module> module)
{
    mModules.push_back(std::move(module));
}
