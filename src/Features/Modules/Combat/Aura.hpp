#pragma once
//
// Created by vastrakai on 7/8/2024.
//

#include <Features/Modules/Module.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>


class Aura : public ModuleBase<Aura> {
public:
    enum class Mode {
        Single,
        Multi,
        Switch
    };

    enum class RotateMode {
        None,
        Normal,
    };

    enum class AttackMode {
        Earliest,
        Synched
    };

    EnumSetting mMode = EnumSetting("Mode", "The mode of the aura", Mode::Switch, "Single", "Multi", "Switch");
    EnumSetting mAttackMode = EnumSetting("Attack Mode", "The mode of attack", AttackMode::Earliest, "Earliest", "Synched");
    EnumSetting mRotateMode = EnumSetting("Rotate Mode", "The mode of rotation", RotateMode::Normal, "None", "Normal");
    NumberSetting mRange = NumberSetting("Range", "The range at which to attack enemies", 5, 0, 10, 0.01);
    BoolSetting mRandomizeAPS = BoolSetting("Randomize APS", "Whether or not to randomize the APS", false);
    NumberSetting mAPS = NumberSetting("APS", "The amount of attacks per second", 10, 0, 20, 0.01);
    NumberSetting mAPSMin = NumberSetting("APS Min", "The minimum APS to randomize", 10, 0, 20, 0.01);
    NumberSetting mAPSMax = NumberSetting("APS Max", "The maximum APS to randomize", 20, 0, 20, 0.01);
    BoolSetting mStrafe = BoolSetting("Strafe", "Whether or not to strafe around the target", true);

    Aura() : ModuleBase("Aura", "Automatically attacks nearby enemies", ModuleCategory::Combat, 0, false) {
        addSettings(&mMode, &mAttackMode, &mRotateMode, &mRange, &mRandomizeAPS, &mAPS, &mAPSMin, &mAPSMax, &mStrafe);

        VISIBILITY_CONDITION(mAPS, mRandomizeAPS.mValue == false);
        VISIBILITY_CONDITION(mAPSMin, mRandomizeAPS.mValue == true);
        VISIBILITY_CONDITION(mAPSMax, mRandomizeAPS.mValue == true);

        mNames = {
            {Lowercase, "aura"},
            {LowercaseSpaced, "aura"},
            {Normal, "Aura"},
            {NormalSpaced, "Aura"}
        };
    }

    AABB mTargetedAABB = AABB();
    bool mRotating = false;

    void onEnable() override;
    void onDisable() override;
    void rotate(Actor* target);
    void onBaseTickEvent(class BaseTickEvent& event);
    void onPacketOutEvent(class PacketOutEvent& event);

    std::string getSettingDisplay() override {
        return mMode.mValues[mMode.mValue];
    }
};
