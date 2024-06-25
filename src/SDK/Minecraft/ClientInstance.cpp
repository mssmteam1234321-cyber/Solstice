//
// Created by vastrakai on 6/24/2024.
//

#include "ClientInstance.hpp"

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

Actor* ClientInstance::getLocalPlayer()
{
    return MemUtils::CallVFunc<Actor*>(OffsetProvider::ClientInstance_getLocalPlayer, this);
}

BlockSource* ClientInstance::getBlockSource()
{
    return MemUtils::CallVFunc<BlockSource*>(OffsetProvider::ClientInstance_getBlockSource, this);
}

Options* ClientInstance::getOptions()
{
    return MemUtils::CallVFunc<Options*>(OffsetProvider::ClientInstance_getOptions, this);
}

std::string ClientInstance::getScreenName()
{
    return MemUtils::CallVFunc<std::string>(OffsetProvider::ClientInstance_getScreenName, this);
}

void ClientInstance::setDisableInput(bool disable)
{
    MemUtils::CallVFunc<void>(OffsetProvider::ClientInstance_setDisableInput, this, disable);
}

void ClientInstance::getMouseGrabbed()
{
    MemUtils::CallVFunc<void>(OffsetProvider::ClientInstance_getMouseGrabbed, this);
}

void ClientInstance::grabMouse()
{
    MemUtils::CallVFunc<void>(OffsetProvider::ClientInstance_grabMouse, this);
}

void ClientInstance::releaseMouse()
{
    MemUtils::CallVFunc<void>(OffsetProvider::ClientInstance_grabMouse, this);
}

void ClientInstance::playUi(const std::string& soundName, float volume, float pitch)
{   // this is exactly what the game does
    mcGame->playUi(soundName, volume, pitch);
}
