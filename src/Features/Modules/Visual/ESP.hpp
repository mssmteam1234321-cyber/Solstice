#pragma once
//
// Created by vastrakai on 7/7/2024.
//
#include <Features/Modules/Module.hpp>


class ESP : public ModuleBase<ESP>
{
public:
    ESP() : ModuleBase("ESP", "Draws a box around entities", ModuleCategory::Visual, 0, false) {
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