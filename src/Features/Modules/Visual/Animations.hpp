#pragma once
//
// Created by vastrakai on 7/18/2024.
//

#include <Features/Modules/Module.hpp>

class Animations : public ModuleBase<Animations> {
public:
    enum class Animation {
        Default,
        Blocking,
        Test
    };
    EnumSettingT<Animation> mAnimation = EnumSettingT("Animation", "The animation to use", Animation::Blocking, "Default", "Blocking", "Test");
    NumberSetting mSwingTime = NumberSetting("Swing Time", "The time it takes to swing your arm", 6.f, 0.f, 20.0f, 1.f);
    BoolSetting mNoSwitchAnimation = BoolSetting("No Switch Animation", "Disables the switch animation", true);
    BoolSetting mFluxSwing = BoolSetting("Flux Swing", "Flux Client styled swinging", true);
    BoolSetting mCustomSwingAngle = BoolSetting("Custom Swing Angle", "Changes the swing angle", true);
    NumberSetting mSwingAngleSetting = NumberSetting("Swing Angle", "The custom swing angle value. (default: -80)", -31.f, -360.f, 360.f, 0.01f);
    BoolSetting mSmallItems = BoolSetting("Small Items", "Makes items smaller", true);

    Animations() : ModuleBase("Animations", "Change your animations!", ModuleCategory::Visual, 0, false)
    {
        addSetting(&mAnimation);
        addSetting(&mSwingTime);
        addSetting(&mNoSwitchAnimation);
        addSetting(&mFluxSwing);
        addSetting(&mCustomSwingAngle);
        addSetting(&mSwingAngleSetting);
        addSetting(&mSmallItems);

        VISIBILITY_CONDITION(mSwingAngleSetting, mCustomSwingAngle.mValue);

        mNames = {
            {Lowercase, "animations"},
            {LowercaseSpaced, "animations"},
            {Normal, "Animations"},
            {NormalSpaced, "Animations"}
        };
    }

    int mSwingDuration = 0.f;
    int mOldSwingDuration = 0.f;
    float* mSwingAngle = nullptr;

    void onEnable() override;
    void onDisable() override;
    void onBaseTickEvent(class BaseTickEvent& event);
    void onSwingDurationEvent(class SwingDurationEvent& event);
    void onBobHurtEvent(class BobHurtEvent& event);
};