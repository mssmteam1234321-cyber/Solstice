#pragma once
#include <Features/Modules/Module.hpp>
//
// Created by vastrakai on 7/2/2024.
//


class LevelInfo : public ModuleBase<LevelInfo>
{
public:
    BoolSetting mShowFPS = BoolSetting("Show FPS", "Show the current FPS", true);
    BoolSetting mShowPing = BoolSetting("Show Ping", "Show the current ping", true);
    BoolSetting mShowBPS = BoolSetting("Show BPS", "Show the current BPS", true);
    BoolSetting mShowXYZ = BoolSetting("Show XYZ", "Show the current XYZ", true);

    LevelInfo() : ModuleBase("LevelInfo", "Display general information", ModuleCategory::Visual, 0, true) {
        addSetting(&mShowFPS);
        addSetting(&mShowPing);
        addSetting(&mShowBPS);
        addSetting(&mShowXYZ);

        mNames = {
            {Lowercase, "levelinfo"},
            {LowercaseSpaced, "level info"},
            {Normal, "LevelInfo"},
            {NormalSpaced, "Level Info"}
        };
    }

    float mBps = 0.f;
    float mAveragedBps = 0.f;
    std::map<uint64_t, float> mBpsHistory;
    __int64 mPing = 0;

    void onEnable() override;
    void onDisable() override;
    void onPingUpdateEvent(class PingUpdateEvent& event);
    void onBaseTickEvent(class BaseTickEvent& event);
    void onRenderEvent(class RenderEvent& event);
};