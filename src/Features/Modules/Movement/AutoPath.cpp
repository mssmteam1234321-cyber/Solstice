//
// Created by vastrakai on 10/1/2024.
//

#include "AutoPath.hpp"
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/World/BlockLegacy.hpp>
#include <SDK/Minecraft/World/HitResult.hpp>

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

__forceinline float AutoPath::heuristicEstimation(const BlockPos& node, const BlockPos& target) {
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

inline bool AutoPath::isCompletelyObstructed(const BlockPos& pos) {
    const auto block = cachedSrc->getBlock(pos);

    if (block->toLegacy()->getmMaterial()->getmIsBlockingMotion() || block->toLegacy()->getmSolid()|| block->toLegacy()->getmMaterial()->getmIsBlockingPrecipitation() || block->toLegacy()->getBlockId() != 0)
        return true;

    return false;
}

std::vector<std::pair<BlockPos, float>> AutoPath::getAirAdjacentNodes(const BlockPos& node, const BlockPos& start, BlockPos& goal) {
    std::vector<std::pair<BlockPos, float>> res;
    res.reserve(10);

    bool sideWorks[5];

    for (int i = 0; i < sideAdj.size(); i++) {
        bool works = false;
        const auto curr = node + sideAdj.at(i);

        if (glm::distance(glm::vec3(curr), glm::vec3(start)) <= 100.f) {
            if (!isCompletelyObstructed(curr) && !isCompletelyObstructed(curr - BlockPos(0, 1, 0))) {
                res.emplace_back(curr, 1.f);
                works = true;
            }
        }

        sideWorks[i] = works;
    }

    // Top
    BlockPos curr = node + BlockPos(0, 1, 0);

    if (glm::distance(glm::vec3(curr), glm::vec3(start)) <= 100.f) {
        if (!isCompletelyObstructed(curr))
            res.emplace_back(curr, 1.f);
    }

    // Bottom
    curr = node - BlockPos(0, 2, 0);

    if (glm::distance(glm::vec3(curr), glm::vec3(start)) <= 100.f) {
        if (!isCompletelyObstructed(curr))
            res.emplace_back(curr + BlockPos(0, 1, 0), 1.f);
    }

    // Diagonal
    sideWorks[4] = sideWorks[0];

    for (int i = 0; i < 4; i++) {
        if (sideWorks[i] && sideWorks[i + 1]) {
            curr = node + BlockPos(diagAdd.at(i));

            if (glm::distance(glm::vec3(curr), glm::vec3(start)) <= 100.f) {
                if (!isCompletelyObstructed(curr) && !isCompletelyObstructed(curr - BlockPos(0, 1, 0)))
                    res.emplace_back(curr, SQRT_2);
            }
        }
    }

    return res;
}

std::vector<glm::vec3> AutoPath::findFlightPath(BlockPos start, BlockPos goal, BlockSource* src, float howClose, bool optimizePath) {
    cachedSrc = src;

    std::map<BlockPos, BlockPos> cameFrom;
    std::map<BlockPos, ScoreList> scores;

    auto cmp = [&scores](const BlockPos& a, const BlockPos& b) {
        return scores[a].fScore < scores[b].fScore;
    };

    std::multiset<BlockPos, decltype(cmp)> openSet(cmp);  // this should be a priority queue or other minheap at some point
    float startH = heuristicEstimation(start, goal);

    scores[start] = ScoreList(0.f, startH);
    openSet.insert(start);

    glm::vec3 closestPoint = glm::vec3(start);
    float bestHeuristic = startH;
    auto startTime = NOW;

    while (!openSet.empty()) {
        BlockPos current = *openSet.begin();

        openSet.erase(openSet.begin());

        if (heuristicEstimation(current, goal) <= howClose || NOW - startTime > 90 /*90 ms padding*/) {
            std::vector path = { closestPoint };
            BlockPos currentReconstructionNode = closestPoint;

            // unoptimised
            while (cameFrom.contains(currentReconstructionNode)) {
                currentReconstructionNode = cameFrom[currentReconstructionNode];
                path.push_back(glm::vec3(currentReconstructionNode));
            }

            std::ranges::reverse(path.begin(), path.end());

            if (!path.empty()) {
                if (heuristicEstimation(current, goal) <= howClose)
                    ChatUtils::displayClientMessage("§aFound path!");
                else
                    ChatUtils::displayClientMessage("§6Found partial path!");


                if (optimizePath && path.size() >= 2) {
                    // ass post processing
                    int startIdx = 0;
                    int endIdx = path.size() - 1;

                    while (startIdx < path.size() - 1) {
                        while (endIdx - startIdx > 1) {
                            // Check line of sight from start to end
                            bool hasLineOfSight = true;
                            glm::vec3* list = getPlayerHitboxPathPosOffsets();

                            for (int li = 0; li < 8; li++) {
                                // Check line of sight for each point
                                glm::vec3 startCheck = path.at(startIdx) + (list[li]);
                                glm::vec3 endCheck = path.at(endIdx) + (list[li]);
                                HitResult rt = src->checkRayTrace(startCheck, endCheck);

                                if (rt.mType == HitType::BLOCK) {
                                    hasLineOfSight = false;
                                    break;
                                }
                            }

                            if (hasLineOfSight) {
                                startIdx++;
                                path.erase(path.begin() + startIdx, path.begin() + endIdx);
                                endIdx = path.size() - 1;
                            }
                            else
                                endIdx--;
                        }

                        startIdx++;
                        endIdx = path.size() - 1;
                    }
                }
            }

            return path;
        }

        for (auto& [pos, f] : getAirAdjacentNodes(current, start, goal)) {
            const float tentative_gScore = scores[current].gScore + f;

            if (tentative_gScore < scores[pos].gScore) {
                // better path found
                cameFrom[pos] = current;

                float h = heuristicEstimation(pos, goal);

                scores[pos] = ScoreList(tentative_gScore, tentative_gScore + h);
                openSet.insert(pos);

                if (h < bestHeuristic) {
                    bestHeuristic = h;
                    closestPoint = glm::vec3(pos);
                }
            }
        }
    }

    ChatUtils::displayClientMessage("§cNo path found!");
    return {};
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
