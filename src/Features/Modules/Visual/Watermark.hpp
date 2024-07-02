#pragma once
//
// Created by vastrakai on 6/29/2024.
//

#include <Solstice.hpp>
#include <Features/FeatureManager.hpp>
#include <Features/Modules/Setting.hpp>

#include "spdlog/spdlog.h"


class Watermark : public ModuleBase<Watermark> {
public:
    BoolSetting mGlow = BoolSetting("Glow", "Enables glow", true);
    Watermark() : ModuleBase("Watermark", "Displays a watermark on the screen", ModuleCategory::Visual, 0, true) {
        addSetting(&mGlow);
    }

    void onEnable() override;
    void onDisable() override;

    void onRenderEvent(class RenderEvent& event);
};

REGISTER_MODULE(Watermark);