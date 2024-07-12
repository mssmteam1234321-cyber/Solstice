#pragma once
//
// Created by vastrakai on 7/10/2024.
//
#include <Features/FeatureManager.hpp>
#include <Features/Modules/Module.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/Network/Packets/MobEffectPacket.hpp>

enum class JumpType {
    Vanilla,
    Velocity,
    None
};


struct FrictionPreset
{
    float speed = 0.5;
    float friction = 0.975;
    bool timerBoost = false;
    float timerSpeed = 20;
    bool fastFall = false;
    int fallTicks = 5;
    float fallSpeed = 1.00;
    JumpType jumpType = JumpType::Vanilla;
    float jumpHeight = 0.42f;

    FrictionPreset() = default;
    FrictionPreset(float speed, float friction, bool timerBoost, float timerSpeed, bool fastFall, int fallTicks, float fallSpeed, JumpType jumpType, float jumpHeight)
        : speed(speed), friction(friction), timerBoost(timerBoost), timerSpeed(timerSpeed), fastFall(fastFall), fallTicks(fallTicks), fallSpeed(fallSpeed), jumpType(jumpType), jumpHeight(jumpHeight)
    {
    }
};

class Speed : public ModuleBase<Speed> {
public:
    enum class Mode {
        Friction,
        FlareonV1
    };

    enum class FlareonPreset {
        FastFall,
        Normal,
        Low1,
    };


    EnumSetting mMode = EnumSetting("Mode", "The mode of speed", Mode::Friction, "Friction", "Flareon V1");
    EnumSetting mFlareonPreset = EnumSetting("Type", "The preset for Flareon", FlareonPreset::FastFall, "FastFall", "Normal", "Low1");
    BoolSetting mSwiftness = BoolSetting("Swiftness", "Whether or not to apply swiftness when space is pressed (will not be applied when scaffold is enabled)", false);
    BoolSetting mHoldSpace = BoolSetting("Hold Space", "Only applies swiftness effect while holding space", false);
    NumberSetting mSwiftnessSpeed = NumberSetting("Swiftness Speed", "The speed to apply when swiftness is active", 0.55, 0, 1, 0.01);
    NumberSetting mSwiftnessFriction = NumberSetting("Swiftness Friction", "The friction to apply when swiftness is active", 0.975, 0, 1, 0.01);
    NumberSetting mSpeed = NumberSetting("Speed", "The speed to move at", 0.5, 0, 10, 0.01);
    NumberSetting mFriction = NumberSetting("Friction", "The friction to apply", 0.975, 0, 1, 0.01);
    BoolSetting mTimerBoost = BoolSetting("Timer Boost", "Whether or not to boost timer speed", false);
    NumberSetting mTimerSpeed = NumberSetting("Timer Speed", "The speed to boost timer by", 20, 0, 40, 0.1);
    BoolSetting mFastFall = BoolSetting("Fast Fall", "Whether or not to fast fall", false);
    NumberSetting mFallTicks = NumberSetting("Fall Ticks", "The tick to apply down motion at", 5, 0, 20, 1);
    NumberSetting mFallSpeed = NumberSetting("Fall Speed", "The speed to fall down at", 1.00, 0, 10, 1);
    EnumSetting mJumpType = EnumSetting("Jump Type", "The type of jump to use", JumpType::Vanilla, "Vanilla", "Velocity", "None");
    NumberSetting mJumpHeight = NumberSetting("Jump Height", "The height to jump at", 0.42f, 0, 1, 0.01);
    BoolSetting mApplyNetskip = BoolSetting("Apply Netskip", "Apply Netskip", false);

    Speed() : ModuleBase("Speed", "Move faster", ModuleCategory::Movement, 0, false) {
        addSettings(&mMode, &mFlareonPreset, &mSwiftness, &mHoldSpace, &mSwiftnessSpeed, &mSwiftnessFriction, &mSpeed, &mFriction, &mTimerBoost, &mTimerSpeed, &mFastFall, &mFallTicks, &mFallSpeed, &mJumpType, &mJumpHeight, &mApplyNetskip);

        VISIBILITY_CONDITION(mSwiftnessSpeed, mSwiftness.mValue);
        VISIBILITY_CONDITION(mSwiftnessFriction, mSwiftness.mValue);
        VISIBILITY_CONDITION(mHoldSpace, mSwiftness.mValue);


        VISIBILITY_CONDITION(mFlareonPreset, mMode.as<Mode>() == Mode::FlareonV1);
        VISIBILITY_CONDITION(mSpeed, mMode.as<Mode>() == Mode::Friction);
        VISIBILITY_CONDITION(mFriction, mMode.as<Mode>() == Mode::Friction);
        VISIBILITY_CONDITION(mTimerBoost, mMode.as<Mode>() == Mode::Friction);
        VISIBILITY_CONDITION(mTimerSpeed, mMode.as<Mode>() == Mode::Friction && mTimerBoost.mValue);
        VISIBILITY_CONDITION(mFastFall, mMode.as<Mode>() == Mode::Friction);
        VISIBILITY_CONDITION(mFallTicks, mMode.as<Mode>() == Mode::Friction && mFastFall.mValue);
        VISIBILITY_CONDITION(mFallSpeed, mMode.as<Mode>() == Mode::Friction && mFastFall.mValue);
        VISIBILITY_CONDITION(mJumpType, mMode.as<Mode>() == Mode::Friction);
        VISIBILITY_CONDITION(mJumpHeight, mMode.as<Mode>() == Mode::Friction && mJumpType.as<JumpType>() == JumpType::Velocity);
        VISIBILITY_CONDITION(mApplyNetskip, mMode.as<Mode>() == Mode::Friction);

        mNames = {
            {Lowercase, "speed"},
            {LowercaseSpaced, "speed"},
            {Normal, "Speed"},
            {NormalSpaced, "Speed"}
        };

        gFeatureManager->mDispatcher->listen<PacketInEvent, &Speed::onPacketInEvent>(this);
    }

    std::map<EffectType, uint64_t> mEffectTimers = {};

    void onEnable() override;
    void onDisable() override;
    void onRunUpdateCycleEvent(class RunUpdateCycleEvent& event);
    bool tickSwiftness();
    void onBaseTickEvent(class BaseTickEvent& event);
    void onPacketInEvent(class PacketInEvent& event);
    void onPacketOutEvent(class PacketOutEvent& event);
    void tickFriction(Actor* player);
    void tickFrictionPreset(FrictionPreset& preset);

    std::string getSettingDisplay() override
    {
        return mMode.mValues[mMode.mValue];
    }
};
