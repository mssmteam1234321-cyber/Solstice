#pragma once
//
// Created by vastrakai on 7/22/2024.
//

#include <Features/Modules/Module.hpp>

class Freecam : public ModuleBase<Freecam> {
public:
    NumberSetting mSpeed = NumberSetting("Speed", "Speed of the freecam", 1.0f, 0.1f, 10.0f, 0.1f);
    BoolSetting mTest = BoolSetting("Test", "Test setting", false);
    Freecam() : ModuleBase<Freecam>("Freecam", "Move independently of your player", ModuleCategory::Player, 0, false) {
        addSetting(&mSpeed);
        addSetting(&mTest);

        mNames = {
            {Lowercase, "freecam"},
            {LowercaseSpaced, "freecam"},
            {Normal, "Freecam"},
            {NormalSpaced, "Freecam"}
        };
    }

    void onEnable() override;
    void onDisable() override;
    void onPacketInEvent(class PacketInEvent& event);
    void onBaseTickEvent(class BaseTickEvent& event);
    void onLookInputEvent(class LookInputEvent& event);

};