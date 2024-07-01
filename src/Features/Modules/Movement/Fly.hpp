#pragma once
//
// Created by vastrakai on 6/30/2024.
//

#include <Features/Modules/Setting.hpp>

#include "../Module.hpp"

class Fly : public ModuleBase<Fly> {
public:
    enum Mode {
        Motion
    };

    EnumSetting Mode = EnumSetting("Mode", "The mode of the fly", Motion, { "Motion" });
    NumberSetting Speed = NumberSetting("Speed", "The speed of the fly", 1.f, 0.f, 10.f, 0.1f);
    BoolSetting ApplyGlideFlags = BoolSetting("Apply Glide Flags", "Applies glide flags to the player", true);

    Fly() : ModuleBase("Fly", "Allows you to fly", ModuleCategory::Movement, 'C', false) {
        addSetting(&Mode);
        addSetting(&Speed);
        addSetting(&ApplyGlideFlags);
        ApplyGlideFlags.mIsVisible = {
            [this]() -> bool {
                return Mode.mValue == Motion;
            }

        };
    }

    void onEnable() override;
    void onDisable() override;
    void onBaseTickEvent(class BaseTickEvent& event) const;
    void onPacketOutEvent(class PacketOutEvent& event) const;
};