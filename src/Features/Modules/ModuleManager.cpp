//
// Created by vastrakai on 6/28/2024.
//

#include "ModuleManager.hpp"

#include <Utils/StringUtils.hpp>

#include "Misc/TestModule.hpp"
#include "Misc/ToggleSounds.hpp"
#include "Movement/Fly.hpp"
#include "spdlog/spdlog.h"
#include "Visual/AutoScale.hpp"
#include "Visual/ClickGui.hpp"
#include "Visual/Watermark.hpp"

void ModuleManager::init()
{
    mModules.emplace_back(std::make_shared<TestModule>());
    mModules.emplace_back(std::make_shared<ToggleSounds>());
    mModules.emplace_back(std::make_shared<Watermark>());
    mModules.emplace_back(std::make_shared<ClickGui>());
    mModules.emplace_back(std::make_shared<AutoScale>());
    mModules.emplace_back(std::make_shared<Fly>());


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

std::vector<std::shared_ptr<Module>>& ModuleManager::getModulesInCategory(int catId)
{
    static std::unordered_map<int, std::vector<std::shared_ptr<Module>>> categoryMap = {};
    if (categoryMap.contains(catId))
    {
        return categoryMap[catId];
    }

    // Cache category
    std::vector<std::shared_ptr<Module>> modules;
    for (const auto& module : mModules)
    {
        if (static_cast<int>(module->mCategory) == catId)
        {
            modules.push_back(module);
        }
    }

    categoryMap[catId] = modules;
    return categoryMap[catId];
}

std::unordered_map<std::string, std::shared_ptr<Module>> ModuleManager::getModuleCategoryMap()
{
    static std::unordered_map<std::string, std::shared_ptr<Module>> map;

    if (!map.empty())
    {
        return map;
    }

    for (const auto& module : mModules)
    {
        map[module->getCategory()] = module;
    }

    return map;
}
