//
// Created by vastrakai on 7/3/2024.
//

#include "BlockLegacy.hpp"

#include <SDK/OffsetProvider.hpp>
#include <SDK/SigManager.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>

#include "BlockSource.hpp"

uint16_t BlockLegacy::getBlockId()
{
    return hat::member_at<uint16_t>(this, OffsetProvider::BlockLegacy_mBlockId);
}

bool BlockLegacy::mayPlaceOn(glm::ivec3 pos)
{
    BlockSource* blockSource = ClientInstance::get()->getBlockSource();
    static auto vIndex = OffsetProvider::BlockLegacy_mayPlaceOn;
    return MemUtils::callVirtualFunc<bool, void*, const glm::ivec3&>(vIndex, this, blockSource, pos);
}

bool BlockLegacy::isAir()
{
    return getBlockId() == 0;
}
