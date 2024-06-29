//
// Created by vastrakai on 6/28/2024.
//

#include "ModuleManager.hpp"

#include <Utils/StringUtils.hpp>

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

Module* ModuleManager::getModule(const std::string& name)
{
    for (const auto& module : mModules)
    {
        if (StringUtils::equalsIgnoreCase(module->mName, name))
        {
            return module.get();
        }
    }
    return nullptr;
}

template <typename T>
T* ModuleManager::getModule()
{
    // TypeID based search
    const auto& typeID = typeid(T).name();
    for (const auto& module : mModules)
    {
        if (module->getTypeID() == typeID)
        {
            return static_cast<T*>(module.get());
        }
    }
    return nullptr;
}