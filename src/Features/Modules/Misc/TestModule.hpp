//
// Created by vastrakai on 6/28/2024.
//
#pragma once

#include <Features/FeatureManager.hpp>
#include <Features/Modules/Module.hpp>
#include <Features/Modules/ModuleManager.hpp>
#include <spdlog/spdlog.h>
#include <Solstice.hpp>
#include <Features/Modules/Setting.hpp>

class TestModule : public ModuleBase<TestModule> {
public:
    EnumSetting mMode = EnumSetting("Mode", "The mode of the test module.", 0, { "Mode 1", "Mode 2", "Mode 3" });
    ColorSetting mColor = ColorSetting("Color", "The color of the test module.", 0xFFFFFFFF);

    TestModule() : ModuleBase("TestModule", "A module for testing purposes", ModuleCategory::Misc, 0, false) {
        addSetting(&mMode);
        addSetting(&mColor);
        VISIBILITY_CONDITION(mColor, mMode.mValue == 1);

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

REGISTER_MODULE(TestModule);