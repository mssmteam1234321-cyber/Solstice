//
// Created by alteik on 20/10/2024.
//

#pragma once

class NoBadEffects : public ModuleBase<NoBadEffects> {
public:
    NoBadEffects() : ModuleBase<NoBadEffects>("NoFire", "Dont render bad effects", ModuleCategory::Visual, 0, false) {
        mNames = {
            {Lowercase, "nobadeffects"},
            {LowercaseSpaced, "no bad effects"},
            {Normal, "NoBadEffects"},
            {NormalSpaced, "No Bad Effects"}
        };
    }

    void onEnable() override;
    void onDisable() override;
    void onPacketInEvent(class PacketInEvent& event);
};

