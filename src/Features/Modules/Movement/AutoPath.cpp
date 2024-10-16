//
// Created by vastrakai on 10/1/2024.
//

#include "AutoPath.hpp"

#include <SDK/Minecraft/ClientInstance.hpp>

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
