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

    enum class AttackMode {
        Earliest,
        Synched
    };

    enum class RotateMode {
        None,
        Normal,
    };

    enum class SwitchMode {
        None,
        Full,
        Spoof
    };

    enum class AnticheatMode {
        None,
        FlareonV1,
        FlareonV2
    };

    EnumSettingT<Mode> mMode = EnumSettingT("Mode", "The mode of the aura", Mode::Switch, "Single", "Multi", "Switch");
    EnumSettingT<AttackMode> mAttackMode = EnumSettingT("Attack Mode", "The mode of attack", AttackMode::Earliest, "Earliest", "Synched");
    EnumSettingT<RotateMode> mRotateMode = EnumSettingT("Rotate Mode", "The mode of rotation", RotateMode::Normal, "None", "Normal");
    EnumSettingT<SwitchMode> mSwitchMode = EnumSettingT("Switch Mode", "The mode of switching", SwitchMode::None, "None", "Full", "Spoof");
    EnumSettingT<AnticheatMode> mAnticheatMode = EnumSettingT("Anticheat", "The anticheat to bypass", AnticheatMode::None, "None", "FlareonV1", "FlareonV2");
    BoolSetting mAutoFireSword = BoolSetting("Auto Fire Sword", "Whether or not to automatically use the fire sword", false);
    BoolSetting mFireSwordSpoof = BoolSetting("Fire Sword Spoof", "Whether or not to spoof the fire sword", false);
    BoolSetting mHotbarOnly = BoolSetting("Hotbar Only", "Whether or not to only attack with items in the hotbar", false);
    BoolSetting mFistFriends = BoolSetting("Fist Friends", "Whether or not to fist friends", false);
    NumberSetting mRange = NumberSetting("Range", "The range at which to attack enemies", 5, 0, 10, 0.01);
    BoolSetting mRandomizeAPS = BoolSetting("Randomize APS", "Whether or not to randomize the APS", false);
    NumberSetting mAPS = NumberSetting("APS", "The amount of attacks per second", 10, 0, 20, 0.01);
    NumberSetting mAPSMin = NumberSetting("APS Min", "The minimum APS to randomize", 10, 0, 20, 0.01);
    NumberSetting mAPSMax = NumberSetting("APS Max", "The maximum APS to randomize", 20, 0, 20, 0.01);
    BoolSetting mThrowProjectiles = BoolSetting("Throw Projectiles", "Whether or not to throw projectiles at the target", false);
    NumberSetting mThrowDelay = NumberSetting("Throw Delay", "The delay between throwing projectiles (in ticks)", 1, 0, 20, 0.01);
    BoolSetting mAutoBow = BoolSetting("Auto Bow", "Whether or not to automatically shoot the bow", false);
    BoolSetting mStrafe = BoolSetting("Strafe", "Whether or not to strafe around the target", true);

    Aura() : ModuleBase("Aura", "Automatically attacks nearby enemies", ModuleCategory::Combat, 0, false) {
        addSettings(&mMode, &mAttackMode, &mRotateMode, &mSwitchMode, &mAnticheatMode, &mAutoFireSword/*, &mFireSwordSpoof*/, &mHotbarOnly, &mFistFriends, &mRange, &mRandomizeAPS, &mAPS, &mAPSMin, &mAPSMax, &mThrowProjectiles, &mThrowDelay, &mAutoBow, &mStrafe);

        VISIBILITY_CONDITION(mAutoFireSword, mSwitchMode.mValue != SwitchMode::None);
        VISIBILITY_CONDITION(mFireSwordSpoof, mAutoFireSword.mValue == true);
        VISIBILITY_CONDITION(mAPS, mRandomizeAPS.mValue == false);
        VISIBILITY_CONDITION(mAPSMin, mRandomizeAPS.mValue == true);
        VISIBILITY_CONDITION(mAPSMax, mRandomizeAPS.mValue == true);
        VISIBILITY_CONDITION(mThrowDelay, mThrowProjectiles.mValue == true);


        mNames = {
            {Lowercase, "aura"},
            {LowercaseSpaced, "aura"},
            {Normal, "Aura"},
            {NormalSpaced, "Aura"}
        };
    }

    AABB mTargetedAABB = AABB();
    bool mRotating = false;
    bool mHasTarget = false;

    int getSword(Actor* target);
    bool shouldUseFireSword(Actor* target);
    void onEnable() override;
    void onDisable() override;
    void rotate(Actor* target);
    void shootBow(Actor* target);
    void throwProjectiles(Actor* target);
    void onRenderEvent(class RenderEvent& event);
    void onBaseTickEvent(class BaseTickEvent& event);
    void onPacketOutEvent(class PacketOutEvent& event);
    void onBobHurtEvent(class BobHurtEvent& event);
    void onBoneRenderEvent(class BoneRenderEvent& event);
    Actor* findObstructingActor(Actor* player, Actor* target);

    std::string getSettingDisplay() override {
        return mMode.mValues[mMode.as<int>()];
    }
};
