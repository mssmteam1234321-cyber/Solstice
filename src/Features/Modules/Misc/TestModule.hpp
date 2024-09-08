//
// Created by vastrakai on 6/28/2024.
//
#pragma once

#include <Features/Modules/Module.hpp>
#include <Features/Modules/Setting.hpp>

class TestModule : public ModuleBase<TestModule> {
public:
    enum class Mode {
        ClipTest,
        OnGroundSpeedTest,
    };

    EnumSettingT<Mode> mMode = EnumSettingT<Mode>("Mode", "The mode to run the module in", Mode::ClipTest, "ClipTest", "OnGroundSpeedTest");
    BoolSetting mOnGroundOnly = BoolSetting("On Ground Only", "Only run the module when the player is on the ground", false);
    NumberSetting mMaxDistance = NumberSetting("Max Distance", "The maximum distance to run the module at", 5, 0, 30, 0.1);
    BoolSetting mManuallyApplyFlags = BoolSetting("Manually Apply Flags", "Manually apply flags to the player", false);

    TestModule() : ModuleBase("TestModule", "A module for testing purposes", ModuleCategory::Misc, 0, false) {
        addSetting(&mMode);
        addSetting(&mOnGroundOnly);
        addSetting(&mMaxDistance);
        addSetting(&mManuallyApplyFlags);

        VISIBILITY_CONDITION(mOnGroundOnly, mMode.mValue == Mode::ClipTest);
        VISIBILITY_CONDITION(mMaxDistance, mMode.mValue == Mode::ClipTest);
        VISIBILITY_CONDITION(mManuallyApplyFlags, mMode.mValue == Mode::OnGroundSpeedTest);

        mNames = {
            {Lowercase, "testmodule"},
            {LowercaseSpaced, "test module"},
            {Normal, "TestModule"},
            {NormalSpaced, "Test Module"}
        };


    }

    void onEnable() override;
    void onDisable() override;
    void onBaseTickEvent(class BaseTickEvent& event);
    void onPacketOutEvent(class PacketOutEvent& event);
    void onPacketInEvent(class PacketInEvent& event);
    void onLookInputEvent(class LookInputEvent& event);
    void onRenderEvent(class RenderEvent& event);
};