//
// Created by vastrakai on 6/28/2024.
//

#include "ModuleManager.hpp"

#include <Utils/StringUtils.hpp>
#include <Utils/GameUtils/ChatUtils.hpp>

#include "Misc/PacketLogger.hpp"
#include "Misc/TestModule.hpp"
#include "Misc/ToggleSounds.hpp"
#include "Movement/Fly.hpp"
#include "Movement/Velocity.hpp"
#include "Player/Timer.hpp"
#include "spdlog/spdlog.h"
#include "Visual/Arraylist.hpp"
#include "Visual/AutoScale.hpp"
#include "Visual/ClickGui.hpp"
#include "Visual/Interface.hpp"
#include "Visual/LevelInfo.hpp"
#include "Visual/Watermark.hpp"

void ModuleManager::init()
{

    // Combat
    // (none yet)

    // Movement
    mModules.emplace_back(std::make_shared<Fly>());
    mModules.emplace_back(std::make_shared<Velocity>());

    // Player
    mModules.emplace_back(std::make_shared<Timer>());

    // Misc
    mModules.emplace_back(std::make_shared<TestModule>());
    mModules.emplace_back(std::make_shared<ToggleSounds>());
    mModules.emplace_back(std::make_shared<PacketLogger>());

    // Visual
    mModules.emplace_back(std::make_shared<Watermark>());
    mModules.emplace_back(std::make_shared<ClickGui>());
    mModules.emplace_back(std::make_shared<AutoScale>());
    mModules.emplace_back(std::make_shared<Interface>());
    mModules.emplace_back(std::make_shared<Arraylist>());
    mModules.emplace_back(std::make_shared<LevelInfo>());

    for (auto& module : mModules)
    {
        module->onInit();
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

void ModuleManager::onClientTick()
{
    for (auto& module : mModules)
    {
        if (module->mWantedState != module->mEnabled)
        {
            module->mEnabled = module->mWantedState;
            if (module->mEnabled)
            {
                module->onEnable();
            }
            else
            {
                module->onDisable();
            }
        }
    }
}

nlohmann::json ModuleManager::serialize() const
{
    nlohmann::json j;
    j["client"] = "Solstice";
    j["version"] = SOLSTICE_VERSION;
    j["modules"] = nlohmann::json::array();

    for (const auto& module : mModules)
    {
        j["modules"].push_back(module->serialize());
    }

    return j;
}
/*
nlohmann::json Module::serialize()
{
    nlohmann::json j;
    j["enabled"] = mEnabled;
    j["key"] = mKey;
    j["settings"] = nlohmann::json::array();
    for (const auto setting : mSettings)
    {
        if (setting->mType == SettingType::Enum)
        {
            auto* enumSetting = reinterpret_cast<EnumSetting*>(setting);
            // use the serialize() from the enum setting
            j["settings"].push_back(enumSetting->serialize());
        }
        else if (setting->mType == SettingType::Number)
        {
            auto* numberSetting = reinterpret_cast<NumberSetting*>(setting);
            // use the serialize() from the number setting
            j["settings"].push_back(numberSetting->serialize());
        }
        else if (setting->mType == SettingType::Bool)
        {
            auto* boolSetting = reinterpret_cast<BoolSetting*>(setting);
            // use the serialize() from the bool setting
            j["settings"].push_back(boolSetting->serialize());
        }
        else if (setting->mType == SettingType::Color)
        {
            auto* colorSetting = reinterpret_cast<ColorSetting*>(setting);
            // use the serialize() from the color setting
            j["settings"].push_back(colorSetting->serialize());
        }
    }
    return j;
}
*/

void ModuleManager::deserialize(const nlohmann::json& j)
{
    // Get the version of the config
    const std::string version = j["version"];
    std::string currentVersion = SOLSTICE_VERSION;

    if (version != currentVersion)
    {
        spdlog::warn("Config version mismatch. Expected: {}, Got: {}", currentVersion, version);
        ChatUtils::displayClientMessage("§eWarning: The specified config is from a different version of Solstice. §cSome settings may not be loaded§e.");
    }

    for (const auto& module : j["modules"])
    {
        const std::string name = module["name"];
        const bool enabled = module["enabled"];
        const int keybind = module["key"];
        //const bool hideInArraylist = module["hideInArraylist"];

        auto* mod = getModule(name);
        if (mod)
        {
            mod->mWantedState = enabled;
            mod->mKey = keybind;
            //mod->mHideInArraylist = hideInArraylist;

            if (module.contains("settings"))
            {
                for (const auto& setting : module["settings"].items())
                {
                    const auto& settingValue = setting.value();
                    const std::string settingName = settingValue["name"];

                    auto* set = mod->getSetting(settingName);
                    if (set)
                    {
                        if (set->mType == SettingType::Bool)
                        {
                            auto* boolSetting = static_cast<BoolSetting*>(set);
                            boolSetting->mValue = settingValue["boolValue"];
                        }
                        else if (set->mType == SettingType::Number)
                        {
                            auto* numberSetting = static_cast<NumberSetting*>(set);
                            numberSetting->mValue = settingValue["numberValue"];
                        }
                        else if (set->mType == SettingType::Enum)
                        {
                            auto* enumSetting = static_cast<EnumSetting*>(set);
                            enumSetting->mValue = settingValue["enumValue"];
                        } else if (set->mType == SettingType::Color)
                        {
                            auto* colorSetting = static_cast<ColorSetting*>(set);
                            // Get the  settingValue["colorValue"] as a float[4]
                            for (int i = 0; i < 4; i++)
                            {
                                colorSetting->mValue[i] = settingValue["colorValue"][i];
                            }
                        }
                    } else
                    {
                        spdlog::warn("Setting {} not found for module {}", settingName, name);
                    }
                }
            }
        }
    }
}