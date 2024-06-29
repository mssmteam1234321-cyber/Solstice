#pragma once
//
// Created by vastrakai on 6/29/2024.
//

#include <Solstice.hpp>
#include <Features/FeatureManager.hpp>

#include "spdlog/spdlog.h"

class ToggleSounds : public ModuleBase<ToggleSounds> {
public:
    ToggleSounds() : ModuleBase("ToggleSounds", "Plays a sound on module toggle", ModuleCategory::Misc, 0, false) {
        // Register your features here
    }

    void onEnable() override;
    void onDisable() override;

    void onModuleStateChange(ModuleStateChangeEvent& event);
};

REGISTER_MODULE(ToggleSounds);