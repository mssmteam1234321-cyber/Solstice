//
// Created by vastrakai on 6/24/2024.
//

#include "ClientInstance.hpp"

#include <libhat/Access.hpp>
#include <SDK/OffsetProvider.hpp>

#include "MinecraftGame.hpp"

ClientInstance* ClientInstance::get()
{
    static MinecraftGame* game = MinecraftGame::getInstance();
    if (game == nullptr)
    {
        game = MinecraftGame::getInstance();
        return nullptr;
    }
    static ClientInstance* instance = game->getPrimaryClientInstance();
    if (instance == nullptr) instance = game->getPrimaryClientInstance();
    return instance;
}

MinecraftSim* ClientInstance::getMinecraftSim()
{
    return hat::member_at<MinecraftSim*>(this, OffsetProvider::ClientInstance_mMinecraftSim);
}

LevelRenderer* ClientInstance::getLevelRenderer()
{
    return hat::member_at<LevelRenderer*>(this, OffsetProvider::ClientInstance_mLevelRenderer);
}

PacketSender* ClientInstance::getPacketSender()
{
    return hat::member_at<PacketSender*>(this, OffsetProvider::ClientInstance_mPacketSender);
}

GuiData* ClientInstance::getGuiData()
{
    return hat::member_at<GuiData*>(this, OffsetProvider::ClientInstance_mGuiData);
}

MinecraftGame* ClientInstance::getMinecraftGame()
{
    return MinecraftGame::getInstance();
}

Actor* ClientInstance::getLocalPlayer()
{
    return MemUtils::callVirtualFunc<Actor*>(OffsetProvider::ClientInstance_getLocalPlayer, this);
}

BlockSource* ClientInstance::getBlockSource()
{
    return MemUtils::callVirtualFunc<BlockSource*>(OffsetProvider::ClientInstance_getBlockSource, this);
}

Options* ClientInstance::getOptions()
{
    return MemUtils::callVirtualFunc<Options*>(OffsetProvider::ClientInstance_getOptions, this);
}

std::string ClientInstance::getScreenName()
{
    std::string name = "no_screen";
    name = MemUtils::callVirtualFunc<std::string&, std::string&>(OffsetProvider::ClientInstance_getScreenName, this, name);
    return name;
}

void ClientInstance::setDisableInput(bool disable)
{
    MemUtils::callVirtualFunc<void>(OffsetProvider::ClientInstance_setDisableInput, this, disable);
}

bool ClientInstance::getMouseGrabbed()
{
    return MemUtils::callVirtualFunc<bool>(OffsetProvider::ClientInstance_getMouseGrabbed, this);
}

void ClientInstance::grabMouse()
{
    MemUtils::callVirtualFunc<void>(OffsetProvider::ClientInstance_grabMouse, this);
}

void ClientInstance::releaseMouse()
{
    MemUtils::callVirtualFunc<void>(OffsetProvider::ClientInstance_grabMouse, this);
}

void ClientInstance::playUi(const std::string& soundName, float volume, float pitch)
{   // this is exactly what the game does
    getMinecraftGame()->playUi(soundName, volume, pitch);
}
