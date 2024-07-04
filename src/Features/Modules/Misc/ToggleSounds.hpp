#pragma once
//
// Created by vastrakai on 6/29/2024.
//

#include <Solstice.hpp>
#include <Features/FeatureManager.hpp>

#include "spdlog/spdlog.h"

class ToggleSounds : public ModuleBase<ToggleSounds> {
public:
    enum Sound {
        Lever,
        Smooth
    };
    EnumSetting mSound = EnumSetting("Sound", "The sound to play on module toggle", Sound::Lever, "Lever", "Smooth");
    ToggleSounds() : ModuleBase("ToggleSounds", "Plays a sound on module toggle", ModuleCategory::Misc, 0, false) {
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
};

REGISTER_MODULE(ToggleSounds);