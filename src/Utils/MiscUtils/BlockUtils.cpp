//
// Created by vastrakai on 7/7/2024.
//

#include "BlockUtils.hpp"

#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/World/BlockSource.hpp>
#include <omp.h>

std::vector<BlockInfo> BlockUtils::getBlockList(const glm::ivec3& position, float r)
{
    std::vector<BlockInfo> blocks = {};

    const auto blockSource = ClientInstance::get()->getBlockSource();

    auto newBlocks = std::vector<BlockInfo>();


    const int radius = static_cast<int>(r);
    newBlocks.reserve(radius * radius * radius); // reserve enough space for all blocks

    for (int x = position.x - radius; x < position.x + radius; x++)
        for (int y = position.y - radius; y < position.y + radius; y++)
            for (int z = position.z - radius; z < position.z + radius; z++)
                if (const auto block = blockSource->getBlock({ x, y, z }))
                    newBlocks.push_back({ block, { x, y, z } });


    return newBlocks;
}
