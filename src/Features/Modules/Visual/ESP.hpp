#pragma once
//
// Created by vastrakai on 7/7/2024.
//
#include <Features/Modules/Module.hpp>


class ESP : public ModuleBase<ESP>
{
public:
    EnumSetting mStyle = EnumSetting("Style", "The style of the ESP.", 0, { "3D" });
    BoolSetting mRenderFilled = BoolSetting("Render Filled", "Whether or not to render the ESP filled.", true);
    ESP() : ModuleBase("ESP", "Draws a box around entities", ModuleCategory::Visual, 0, false) {
        addSetting(&mStyle);
        addSetting(&mRenderFilled);

        mNames = {
            {Lowercase, "esp"},
            {LowercaseSpaced, "esp"},
            {Normal, "ESP"},
            {NormalSpaced, "ESP"}
        };
    }

    void onEnable() override;
    void onDisable() override;
    void onRenderEvent(class RenderEvent& event);
};