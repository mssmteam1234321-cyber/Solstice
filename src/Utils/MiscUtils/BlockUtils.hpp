#pragma once
//
// Created by vastrakai on 7/7/2024.
//
#include <map>
#include <Utils/Structs.hpp>
#include <SDK/Minecraft/World/Block.hpp>
#include <vector>

struct BlockInfo {
    Block* mBlock;
    glm::ivec3 mPosition;

    AABB getAABB() {
        return AABB(mPosition, glm::vec3(1, 1, 1));
    }

    BlockInfo(Block* block, glm::ivec3 position) : mBlock(block), mPosition(position) {}
};

class BlockUtils {
public:
    static std::vector<BlockInfo> getBlockList(const glm::ivec3& position, float r);

    /*if (!IsAirBlock(pos + BlockPos(0, -1, 0))) return 1;
if (!IsAirBlock(pos + BlockPos(0, 0, 1))) return 2;
if (!IsAirBlock(pos + BlockPos(0, 0, -1))) return 3;
if (!IsAirBlock(pos + BlockPos(1, 0, 0))) return 4;
if (!IsAirBlock(pos + BlockPos(-1, 0, 0))) return 5;
if (!IsAirBlock(pos + BlockPos(0, 1, 0))) return 0;*/
    static inline std::map<int, glm::vec3> blockFaceOffsets = {
        {1, glm::ivec3(0, -1, 0)},
        {2, glm::ivec3(0, 0, 1)},
        {3, glm::ivec3(0, 0, -1)},
        {4, glm::ivec3(1, 0, 0)},
        {5, glm::ivec3(-1, 0, 0)},
        {0, glm::ivec3(0, 1, 0)}
    };

    static int getBlockPlaceFace(glm::ivec3 blockPos);
    static int getExposedFace(glm::ivec3 blockPos);
    static bool isAirBlock(glm::ivec3 blockPos);
    static glm::ivec3 getClosestPlacePos(glm::ivec3 pos, float distance);
    static bool isValidPlacePos(glm::ivec3 pos);
    static void placeBlock(glm::vec3 pos, int side);
    static void startDestroyBlock(glm::vec3 pos, int side);
    static void clearBlock(const glm::ivec3& pos);
    static void destroyBlock(glm::vec3 pos, int side, bool useTransac = false);

};