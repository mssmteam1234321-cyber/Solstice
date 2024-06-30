//
// Created by vastrakai on 6/28/2024.
//

#include "ModuleManager.hpp"

#include <Utils/StringUtils.hpp>

#include "Misc/TestModule.hpp"
#include "Misc/ToggleSounds.hpp"
#include "spdlog/spdlog.h"
#include "Visual/Watermark.hpp"

void ModuleManager::init()
{
    mModules.emplace_back(std::make_shared<TestModule>());
    mModules.emplace_back(std::make_shared<ToggleSounds>());
    mModules.emplace_back(std::make_shared<Watermark>());

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

void ModuleManager::registerModule(const std::shared_ptr<Module>& module)
{
    mModules.push_back(module);
}

std::vector<std::shared_ptr<Module>>& ModuleManager::getModules()
{
    return mModules;
}

Module* ModuleManager::getModule(const std::string& name) const
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