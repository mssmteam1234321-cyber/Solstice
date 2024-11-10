#pragma once
//
// Created by vastrakai on 10/1/2024.
//


class AutoPath : public ModuleBase<AutoPath>
{
    std::vector<glm::vec3> mPosList{};
    int mTicks = 0;
    std::mutex mMutex;

public:
    NumberSetting mHowClose = NumberSetting("How Close", "deborg", 1.f, 0.f, 10.f, 0.01f);

    AutoPath() : ModuleBase("AutoPath", "Automatically pathfinds to a specified location", ModuleCategory::Movement, 0, false) {
        addSettings(&mHowClose);

        mNames = {
            {Lowercase, "autopath"},
            {LowercaseSpaced, "auto path"},
            {Normal, "AutoPath"},
            {NormalSpaced, "Auto Path"}
        };
    }

    static BlockSource* cachedSrc;
    static const float SQRT_2;
    static std::vector<BlockPos> sideAdj;
    static std::vector<BlockPos> directAdj;
    static std::vector<BlockPos> diagAdd;
    static float heuristicEstimation(const BlockPos& node, const BlockPos& target);
    static bool isCompletelyObstructed(const BlockPos& pos);
    static std::vector<std::pair<BlockPos, float>> getAirAdjacentNodes(const BlockPos& node, const BlockPos& start, BlockPos& goal);
    static std::vector<glm::vec3> findFlightPath(BlockPos start, BlockPos goal, BlockSource* src, float howClose, bool optimizePath);
    void onEnable() override;
    void onDisable() override;
    void onBaseTickEvent(class BaseTickEvent& event);
    void onRenderEvent(RenderEvent& event);
};