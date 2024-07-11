#pragma once
//
// Created by vastrakai on 7/10/2024.
//
#include <Features/Modules/Module.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>

class Speed : public ModuleBase<Speed> {
public:
    enum class Mode {
        Friction,
        FlareonV1
    };

    enum class FlareonPreset {
        Normal,
    };

    enum class JumpType {
        Vanilla,
        Velocity,
        None
    };


    EnumSetting mMode = EnumSetting("Mode", "The mode of speed", Mode::Friction, "Friction", "Flareon V1");
    EnumSetting mFlareonPreset = EnumSetting("Type", "The preset for Flareon", FlareonPreset::Normal, "Normal");
    NumberSetting mSpeed = NumberSetting("Speed", "The speed to move at", 0.5, 0, 10, 0.01);
    NumberSetting mFriction = NumberSetting("Friction", "The friction to apply", 0.975, 0, 1, 0.01);
    BoolSetting mTimerBoost = BoolSetting("Timer Boost", "Whether or not to boost timer speed", false);
    NumberSetting mTimerSpeed = NumberSetting("Timer Speed", "The speed to boost timer by", 20, 0, 40, 0.1);
    EnumSetting mJumpType = EnumSetting("Jump Type", "The type of jump to use", JumpType::Vanilla, "Vanilla", "Velocity", "None");
    NumberSetting mJumpHeight = NumberSetting("Jump Height", "The height to jump at", 0.42f, 0, 1, 0.01);
    BoolSetting mApplyNetskip = BoolSetting("Apply Netskip", "Apply Netskip", false);

    Speed() : ModuleBase("Speed", "Move faster", ModuleCategory::Movement, 0, false) {
        addSettings(&mMode, &mFlareonPreset, &mSpeed, &mFriction, &mTimerBoost, &mTimerSpeed, &mJumpType, &mJumpHeight, &mApplyNetskip);

        VISIBILITY_CONDITION(mFlareonPreset, mMode.as<Mode>() == Mode::FlareonV1);
        VISIBILITY_CONDITION(mSpeed, mMode.as<Mode>() == Mode::Friction);
        VISIBILITY_CONDITION(mFriction, mMode.as<Mode>() == Mode::Friction);
        VISIBILITY_CONDITION(mTimerBoost, mMode.as<Mode>() == Mode::Friction);
        VISIBILITY_CONDITION(mTimerSpeed, mMode.as<Mode>() == Mode::Friction && mTimerBoost.mValue);
        VISIBILITY_CONDITION(mJumpType, mMode.as<Mode>() == Mode::Friction);
        VISIBILITY_CONDITION(mJumpHeight, mMode.as<Mode>() == Mode::Friction && mJumpType.as<JumpType>() == JumpType::Velocity);
        VISIBILITY_CONDITION(mApplyNetskip, mMode.as<Mode>() == Mode::Friction);

        mNames = {
            {Lowercase, "speed"},
            {LowercaseSpaced, "speed"},
            {Normal, "Speed"},
            {NormalSpaced, "Speed"}
        };
    }

    void onEnable() override;
    void onDisable() override;
    void onRunUpdateCycleEvent(class RunUpdateCycleEvent& event);
    void onBaseTickEvent(class BaseTickEvent& event);
    void onPacketOutEvent(class PacketOutEvent& event);
    void tickFriction(Actor* player);

    std::string getSettingDisplay() override
    {
        return mMode.mValues[mMode.mValue];
    }
};
