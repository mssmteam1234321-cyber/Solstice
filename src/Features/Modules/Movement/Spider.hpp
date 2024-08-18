//
// Created by ssi on 8/17/2024.
//

#pragma once
#include <Features/Modules/Module.hpp>
#include <Features/Modules/Setting.hpp>

class Spider : public ModuleBase<Spider> {
public:
    enum class Mode {
        Clip
    };

    EnumSettingT<Mode> mMode = EnumSettingT<Mode>("Mode", "Choose the climbing mode to use", Mode::Clip, "Clip");
    NumberSetting mSpeed = NumberSetting("Speed", "Adjust the climbing speed", 2.50, 1, 5, 0.01);

    Spider() : ModuleBase("Spider", "Allows you to climb up walls", ModuleCategory::Movement, 0, false) {
        addSetting(&mMode);
        addSetting(&mSpeed);

        mNames = {
            {Lowercase, "spider"},
            {Normal, "Spider"},
        };
    }

    void onEnable() override;
    void onDisable() override;
    void onBaseTickEvent(class BaseTickEvent& event) const;
};