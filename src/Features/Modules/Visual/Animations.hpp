#pragma once
//
// Created by vastrakai on 7/18/2024.
//

#include <Features/Modules/Module.hpp>

class Animations : public ModuleBase<Animations> {
public:
    NumberSetting mSwingTime = NumberSetting("Swing Time", "The time it takes to swing your arm", 6.f, 0.f, 20.0f, 1.f);
    BoolSetting mNoSwitchAnimation = BoolSetting("No Switch Animation", "Disables the switch animation", false);
    BoolSetting mFluxSwing = BoolSetting("Flux Swing", "Flux Client styled swinging", true);
    BoolSetting mCustomSwingAngle = BoolSetting("Custom Swing Angle", "Changes the swing angle", false);
    NumberSetting mSwingAngle = NumberSetting("Swing Angle", "The custom swing angle value. (default: -80)", -80.f, -360.f, 360.f, 0.01f);

    Animations() : ModuleBase("Animations", "Change your animations!", ModuleCategory::Visual, 0, false)
    {
        addSetting(&mSwingTime);
        addSetting(&mNoSwitchAnimation);
        addSetting(&mFluxSwing);
        addSetting(&mCustomSwingAngle);
        addSetting(&mSwingAngle);

        VISIBILITY_CONDITION(mSwingAngle, mCustomSwingAngle.mValue);

        mNames = {
            {Lowercase, "animations"},
            {LowercaseSpaced, "animations"},
            {Normal, "Animations"},
            {NormalSpaced, "Animations"}
        };
    }

    void onEnable() override;
    void onDisable() override;
    void onBaseTickEvent(class BaseTickEvent& event);
    void onSwingDurationEvent(class SwingDurationEvent& event);
};