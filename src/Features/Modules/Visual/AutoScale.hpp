#pragma once
#include <Features/Modules/Module.hpp>
#include <Features/Modules/ModuleManager.hpp>

#include "glm/vec2.hpp"
//
// Created by vastrakai on 6/30/2024.
//




class AutoScale : public ModuleBase<AutoScale> {
public:
    static inline float Scale = 2.0f; // Placeholder, we don't have settings yet

    AutoScale() : ModuleBase("AutoScale", "Automatically sets the gui scale", ModuleCategory::Visual, 0, false) {

    }

    float mOldScaleMultiplier;
    glm::vec2 mOldScaledResolution;
    float mOldGuiScale;

    void onEnable() override;
    void onDisable() override;
    void onRenderEvent(class RenderEvent& event);
};

REGISTER_MODULE(AutoScale);