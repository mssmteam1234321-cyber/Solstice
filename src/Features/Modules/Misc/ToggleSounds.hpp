#pragma once
//
// Created by vastrakai on 6/29/2024.
//

#include <Features/FeatureManager.hpp>

class ToggleSounds : public ModuleBase<ToggleSounds> {
public:
    enum class Sound {
        Lever,
        Smooth,
        Celestial,
        Nursultan
    };
    EnumSettingT<Sound> mSound = EnumSettingT<Sound>("Sound", "The sound to play on module toggle", Sound::Celestial, "Lever", "Smooth", "Celestial", "Nursultan");
    ToggleSounds() : ModuleBase("ToggleSounds", "Plays a sound on module toggle", ModuleCategory::Misc, 0, true) {
        addSetting(&mSound);

        mNames = {
            {Lowercase, "togglesounds"},
            {LowercaseSpaced, "toggle sounds"},
            {Normal, "ToggleSounds"},
            {NormalSpaced, "Toggle Sounds"}
        };
    }

    void onEnable() override;
    void onDisable() override;

    void onModuleStateChange(ModuleStateChangeEvent& event);

    std::string getSettingDisplay() override {
        return mSound.mValue == Sound::Lever ? "Lever" : "Smooth";
    }
};