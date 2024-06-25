//
// Created by vastrakai on 6/24/2024.
//

#include "MinecraftGame.hpp"
#include <map>
#include <memory>
#include <libhat/Access.hpp>
#include <SDK/OffsetProvider.hpp>
#include <Utils/MemUtils.hpp>

#include "UWP/BedrockPlatformUWP.hpp"
#include "UWP/MainView.hpp"

MinecraftGame* MinecraftGame::getInstance()
{
    if (!MainView::getInstance() || !MainView::getInstance()->getBedrockPlatform() || !MainView::getInstance()->getBedrockPlatform()->getMinecraftGame())
        return nullptr;
    return MainView::getInstance()->getBedrockPlatform()->getMinecraftGame();
    return nullptr;
}

ClientInstance* MinecraftGame::getPrimaryClientInstance()
{
    auto helpme = hat::member_at<std::map<unsigned char, std::shared_ptr<class ClientInstance>>, MinecraftGame>(this, OffsetProvider::MinecraftGame_mClientInstances);

    for (auto& clientInstance : helpme)
    {
        return clientInstance.second.get();
    }

    return nullptr;
}

void MinecraftGame::playUi(const std::string& soundName, float volume, float pitch)
{
    int index = OffsetProvider::MinecraftGame_playUi;
    MemUtils::callVirtualFunc<void, const std::string&, float, float>(index, this, soundName, volume, pitch);
}