#pragma once
//
// Created by vastrakai on 7/7/2024.
//
#include <Utils/Structs.hpp>
#include <SDK/Minecraft/World/Block.hpp>
#include <vector>

struct BlockInfo {
    Block* mBlock;
    glm::ivec3 mPosition;

    AABB getAABB() {
        return AABB(mPosition, glm::vec3(1, 1, 1));
    }
};

class BlockUtils {
public:
    static std::vector<BlockInfo> getBlockList(const glm::ivec3& position, float r);
};