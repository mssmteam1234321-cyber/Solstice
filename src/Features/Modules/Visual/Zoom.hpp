#pragma once

//
// Created by alteik on 15/10/2024.
//

class Zoom : public ModuleBase<Zoom> {
public:
    Zoom() : ModuleBase<Zoom>("Zoom", "decrease ur fov", ModuleCategory::Visual, 0, false) {
        mNames = {
            {Lowercase, "zoom"},
            {LowercaseSpaced, "zoom"},
            {Normal, "Zoom"},
            {NormalSpaced, "Zoom"}
        };

        mEnableWhileHeld = true;
    }

    float mPastFov = 0;

    void onEnable() override;
    void onDisable() override;
};