//
// Created by alteik on 18/10/2024.
//
#pragma once

class NoFire : public ModuleBase<NoFire> {
public:
    NoFire() : ModuleBase<NoFire>("NoFire", "Dont render fire", ModuleCategory::Visual, 0, false) {
        mNames = {
            {Lowercase, "nofire"},
            {LowercaseSpaced, "no fire"},
            {Normal, "NoFire"},
            {NormalSpaced, "No Fire"}
        };
    }

    uintptr_t mFuncAddr = 0x0;
    static inline unsigned char mOriginalData[1];

    void onInit() override;
    void onEnable() override;
    void onDisable() override;
};
