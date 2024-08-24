//
// Created by vastrakai on 8/24/2024.
//

#include "IRC.hpp"

#include <Features/IRC/IrcClient.hpp>

void IRC::onEnable()
{
    IrcManager::init();
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &IRC::onBaseTickEvent>(this);
}

void IRC::onDisable()
{
    IrcManager::deinit();
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &IRC::onBaseTickEvent>(this);
}

void IRC::onBaseTickEvent(BaseTickEvent& event)
{

    if (IrcManager::mClient && IrcManager::mLastConnectAttempt + 5000 < NOW && IrcManager::mClient->mConnectionState == ConnectionState::Disconnected)
    {
        ChatUtils::displayClientMessageRaw("§7[§dirc§7] §eAttempting to reconnect to IRC...");
        IrcManager::init();
    }

}