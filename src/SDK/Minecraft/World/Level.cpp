//
// Created by vastrakai on 7/10/2024.
//

#include "Level.hpp"

#include <SDK/OffsetProvider.hpp>

HitResult* Level::getHitResult()
{
    static auto vIndex = OffsetProvider::Level_getHitResult;
    return MemUtils::callVirtualFunc<HitResult*>(vIndex, this);
}
