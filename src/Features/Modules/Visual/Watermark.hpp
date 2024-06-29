#pragma once
//
// Created by vastrakai on 6/29/2024.
//

#include <Solstice.hpp>
#include <Features/FeatureManager.hpp>

#include "spdlog/spdlog.h"


class Watermark : public ModuleBase<Watermark> {
public:
    Watermark() : ModuleBase("Watermark", "Displays a watermark on the screen", ModuleCategory::Visual, 0, true) {
        // Register your features here
    }

    void onEnable() override;
    void onDisable() override;

    void onRenderEvent(class RenderEvent& event);
};

REGISTER_MODULE(Watermark);