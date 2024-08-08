//
// Created by vastrakai on 7/3/2024.
//

#include "BlockSource.hpp"

#include <memory>
#include <SDK/OffsetProvider.hpp>
#include <SDK/Minecraft/Network/MinecraftPackets.hpp>
#include <SDK/Minecraft/Network/Packets/UpdateBlockPacket.hpp>
#include <Utils/GameUtils/PacketUtils.hpp>

LevelChunk* BlockSource::getChunk(ChunkPos const& pos)
{
    static int index = OffsetProvider::BlockSource_getChunk;
    return MemUtils::callVirtualFunc<LevelChunk*, ChunkPos const&>(index, this, pos);
}
