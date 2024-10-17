//
// Created by vastrakai on 10/1/2024.
//

#include "AutoPath.hpp"
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/World/BlockLegacy.hpp>

BlockSource* AutoPath::cachedSrc = nullptr;
const float AutoPath::SQRT_2 = sqrtf(2.f);

std::vector<BlockPos> AutoPath::sideAdj = {
    {1, 0, 0},
    {0, 0, 1},
    {-1, 0, 0},
    {0, 0, -1}
};

std::vector<BlockPos> AutoPath::directAdj = {
    {1, 0, 0},
    {0, 0, 1},
    {-1, 0, 0},
    {0, 0, -1},
    {0, 1, 0},
    {0, -1, 0}
};

std::vector<BlockPos> AutoPath::diagAdd = {
    {1, 0, 1},
    {-1, 0, 1},
    {-1, 0, -1},
    {1, 0, -1}
};

struct ScoreList {
    float gScore;
    float fScore;

    ScoreList() {
        this->gScore = 694206942069420.f;
        this->fScore = 694206942069420.f;
    }

    ScoreList(const float g, const float f) {
        this->gScore = g;
        this->fScore = f;
    }
};

static glm::vec3* getPlayerHitboxPathPosOffsets() {
    static glm::vec3 res[8] = {
        {0.17f, -0.99f, 0.17f},
        {0.83f, -0.99f, 0.17f},
        {0.83f, -0.99f, 0.83f},
        {0.17f, -0.99f, 0.83f},
        {0.17f, 0.9f, 0.17f},
        {0.83f, 0.9f, 0.17f},
        {0.83f, 0.9f, 0.83f},
        {0.17f, 0.9f, 0.83f}
    };

    return res;
}

__forceinline float AutoPath::heuristicEstimation(const glm::ivec3& node, const glm::ivec3& target) {
    const auto diff = node - target;
    const int x = abs(diff.x);
    const int z = abs(diff.z);
    float straight;
    float diagonal;

    if (x < z) {
        straight = static_cast<float>(z) - static_cast<float>(x);
        diagonal = static_cast<float>(x);
    }
    else {
        straight = static_cast<float>(x) - static_cast<float>(z);
        diagonal = static_cast<float>(z);
    }

    diagonal *= SQRT_2;
    return straight + diagonal + static_cast<float>(abs(target.y - node.y));
}

inline bool AutoPath::isCompletelyObstructed(const glm::ivec3& pos) {
    const auto block = cachedSrc->getBlock(pos);

    if (block->toLegacy()->getmMaterial()->getmIsBlockingMotion() || block->toLegacy()->getmSolid()|| block->toLegacy()->getmMaterial()->getmIsBlockingPrecipitation() || block->toLegacy()->getBlockId() != 0)
        return true;

    return false;
}

void AutoPath::onEnable()
{
    if (ClientInstance::get()->getLocalPlayer() == nullptr)
        setEnabled(false);

    posList.clear();
    ticks = 0;
}

void AutoPath::onDisable()
{

}

void AutoPath::onBaseTickEvent(BaseTickEvent& event)
{
    //todo: implement
}
