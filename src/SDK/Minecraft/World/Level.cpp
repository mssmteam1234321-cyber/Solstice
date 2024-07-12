//
// Created by vastrakai on 7/10/2024.
//

#include "Level.hpp"

#include <libhat.hpp>
#include <SDK/OffsetProvider.hpp>

std::unordered_map<mce::UUID, PlayerListEntry>* Level::getPlayerList()
{
    static auto vIndex = OffsetProvider::Level_getPlayerList;
    return MemUtils::callVirtualFunc<std::unordered_map<mce::UUID, PlayerListEntry>*>(vIndex, this);
}

HitResult* Level::getHitResult()
{
    static auto vIndex = OffsetProvider::Level_getHitResult;
    return MemUtils::callVirtualFunc<HitResult*>(vIndex, this);
}
