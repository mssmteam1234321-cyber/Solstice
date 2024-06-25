//
// Created by vastrakai on 6/24/2024.
//

#include "ClientInstance.hpp"

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
