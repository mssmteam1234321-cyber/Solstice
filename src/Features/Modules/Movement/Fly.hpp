#pragma once
//
// Created by vastrakai on 6/30/2024.
//

#include <Features/Modules/Setting.hpp>

#include "../Module.hpp"

class Fly : public ModuleBase<Fly> {
public:
    enum class Mode {
        Motion,
        Elytra
    };

    EnumSetting Mode = EnumSetting("Mode", "The mode of the fly", Mode::Motion,  "Motion", "Elytra");
    NumberSetting Speed = NumberSetting("Speed", "The speed of the fly", 1.f, 0.f, 20.f, 0.1f);
    BoolSetting ApplyGlideFlags = BoolSetting("Apply Glide Flags", "Applies glide flags to the player", true);

    Fly() : ModuleBase("Fly", "Allows you to fly", ModuleCategory::Movement, 0, false) {
        addSetting(&Mode);
        addSetting(&Speed);
        addSetting(&ApplyGlideFlags);
        VISIBILITY_CONDITION(ApplyGlideFlags, Mode.mValue == static_cast<int>(Mode::Motion));

        mNames = {
            {Lowercase, "fly"},
            {LowercaseSpaced, "fly"},
            {Normal, "Fly"},
            {NormalSpaced, "Fly"}
        };
    }

    void onEnable() override;
    void onDisable() override;
    void onBaseTickEvent(class BaseTickEvent& event) const;
    void onPacketOutEvent(class PacketOutEvent& event) const;

    std::string getSettingDisplay() override
    {
        return Mode.mValues[Mode.mValue];
    }
};