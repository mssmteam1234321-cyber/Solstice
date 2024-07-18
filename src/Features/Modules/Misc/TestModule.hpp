//
// Created by vastrakai on 6/28/2024.
//
#pragma once

#include <Features/Modules/Module.hpp>
#include <Features/Modules/Setting.hpp>

class TestModule : public ModuleBase<TestModule> {
public:
    enum class Mode {
        Mode1,
        Mode2,
        Mode3
    };

    EnumSettingT<Mode> mMode = EnumSettingT<Mode>("Mode", "The mode of the test module.", Mode::Mode1, "Mode1", "Mode2", "Mode3");
    ColorSetting mColor = ColorSetting("Color", "The color of the test module.", 0xFFFFFFFF);

    TestModule() : ModuleBase("TestModule", "A module for testing purposes", ModuleCategory::Misc, 0, false) {
        addSetting(&mMode);
        addSetting(&mColor);
        VISIBILITY_CONDITION(mColor, mMode.mValue == Mode::Mode2);

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
    void onRenderEvent(class RenderEvent& event);
};