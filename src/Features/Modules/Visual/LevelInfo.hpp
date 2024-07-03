#pragma once
#include <Features/Modules/Module.hpp>
//
// Created by vastrakai on 7/2/2024.
//


class LevelInfo : public ModuleBase<LevelInfo>
{
public:
    LevelInfo() : ModuleBase("LevelInfo", "Display general information", ModuleCategory::Visual, 0, true) {

        mNames = {
            {Lowercase, "levelinfo"},
            {LowercaseSpaced, "level info"},
            {Normal, "LevelInfo"},
            {NormalSpaced, "Level Info"}
        };
    }
    void onEnable() override;
    void onDisable() override;

    void onRenderEvent(class RenderEvent& event);
};