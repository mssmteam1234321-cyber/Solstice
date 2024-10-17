#pragma once
//
// Created by vastrakai on 10/1/2024.
//


class AutoPath : public ModuleBase<AutoPath>
{
    std::vector<glm::vec3> posList{};
    int ticks = 0;

public:
    AutoPath() : ModuleBase("AutoPath", "Automatically pathfinds to a specified location", ModuleCategory::Movement, 0, false) {

    }

    static BlockSource* cachedSrc;
    static const float SQRT_2;
    static std::vector<BlockPos> sideAdj;
    static std::vector<BlockPos> directAdj;
    static std::vector<BlockPos> diagAdd;
    static float heuristicEstimation(const glm::ivec3& node, const glm::ivec3& target);
    static bool isCompletelyObstructed(const glm::ivec3& pos);
    void onEnable() override;
    void onDisable() override;
    void onBaseTickEvent(class BaseTickEvent& event);
};