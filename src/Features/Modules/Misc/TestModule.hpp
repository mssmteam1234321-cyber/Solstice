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
    NumberSetting mNumber = NumberSetting("Camera Radius", "The radius of the camera", 4.0f, 0.0f, 10.0f, 0.1f);

    TestModule() : ModuleBase("TestModule", "A module for testing purposes", ModuleCategory::Misc, 0, false) {
        addSetting(&mMode);
        addSetting(&mColor);
        addSetting(&mNumber);

        VISIBILITY_CONDITION(mColor, mMode.mValue == Mode::Mode2);
        VISIBILITY_CONDITION(mNumber, mMode.mValue == Mode::Mode3);

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