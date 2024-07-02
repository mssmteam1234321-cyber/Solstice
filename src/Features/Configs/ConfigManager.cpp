//
// Created by vastrakai on 7/2/2024.
//

#include "ConfigManager.hpp"

#include <fstream>
#include <Features/FeatureManager.hpp>
#include <Utils/FileUtils.hpp>
#include <nlohmann/json.hpp>

#include "spdlog/spdlog.h"

std::string ConfigManager::getConfigPath()
{
    return FileUtils::getSolsticeDir() + "Configs\\";
}

bool ConfigManager::configExists(const std::string& name)
{
    return FileUtils::fileExists(getConfigPath() + name + ".json");
}

void ConfigManager::loadConfig(const std::string& name)
{
    std::ifstream file(getConfigPath() + name + ".json");
    nlohmann::json j;
    file >> j;
    file.close();

    gFeatureManager->mModuleManager->deserialize(j);

    lastLoadedConfig = name;

    spdlog::info("Loaded config " + name + " successfully.");
}

void ConfigManager::saveConfig(const std::string& name)
{
    nlohmann::json j = gFeatureManager->mModuleManager->serialize();

    std::ofstream file(getConfigPath() + name + ".json");
    file << j.dump(4);
    file.close();

    lastLoadedConfig = name;

    spdlog::info("Config saved successfully.");

}
