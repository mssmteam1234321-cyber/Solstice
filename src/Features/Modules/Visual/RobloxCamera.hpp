#pragma once
//
// Created by vastrakai on 7/23/2024.
//

#include <Features/Modules/Module.hpp>

class RobloxCamera : public ModuleBase<RobloxCamera> {
public:
    NumberSetting mRadius = NumberSetting("Radius", "The radius of the camera", 4.0f, 1.0f, 20.0f, 1.0f);
    BoolSetting mScroll = BoolSetting("Scroll", "Scroll to zoom in and out while holding control", true);
    NumberSetting mScrollIncrement = NumberSetting("Scroll Increment", "The amount to zoom in and out", 1.0f, 0.1f, 10.0f, 0.1f);

    RobloxCamera() : ModuleBase("RobloxCamera", "Change the camera to be like Roblox's camera", ModuleCategory::Visual, 0, false) {
        addSetting(&mRadius);
        addSetting(&mScroll);

        mNames = {
            {Lowercase, "robloxcamera"},
            {LowercaseSpaced, "roblox camera"},
            {Normal, "RobloxCamera"},
            {NormalSpaced, "Roblox Camera"}
        };

    }

    float mCurrentDistance = 0.f;

    void onEnable() override;
    void onDisable() override;
    void onMouseEvent(class MouseEvent& event);
    void onLookInputEvent(class LookInputEvent& event);
};