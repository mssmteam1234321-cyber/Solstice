//
// Created by alteik on 18/10/2024.
//
#pragma once

class NoFire : public ModuleBase<NoFire> {
public:
    NoFire() : ModuleBase<NoFire>("NoFire", "Prevents fire animation from being rendered", ModuleCategory::Visual, 0, false) {
        mNames = {
            {Lowercase, "nofire"},
            {LowercaseSpaced, "no fire"},
            {Normal, "NoFire"},
            {NormalSpaced, "No Fire"}
        };
    }

    void onEnable() override;
    void onDisable() override;
};
