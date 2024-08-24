//
// Created by vastrakai on 8/24/2024.
//

#include "IRC.hpp"

#include <Features/IRC/IrcClient.hpp>

void IRC::onEnable()
{
    IrcManager::init();
}

void IRC::onDisable()
{
    IrcManager::deinit();
}
