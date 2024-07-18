#pragma once
//
// Created by vastrakai on 7/18/2024.
//

#include <Features/Modules/Module.hpp>

class Animations : public ModuleBase<Animations> {
public:
    NumberSetting mSwingTime = NumberSetting("Swing Time", "The time it takes to swing your arm", 6.f, 0.f, 20.0f, 1.f);

    Animations() : ModuleBase("Animations", "Change your animations!", ModuleCategory::Visual, 0, false)
    {
        addSetting(&mSwingTime);

        mNames = {
            {Lowercase, "animations"},
            {LowercaseSpaced, "animations"},
            {Normal, "Animations"},
            {NormalSpaced, "Animations"}
        };
    }

    void onEnable() override;
    void onDisable() override;
    void onSwingDurationEvent(class SwingDurationEvent& event);
};