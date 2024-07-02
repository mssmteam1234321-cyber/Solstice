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
    BoolSetting mBool1 = BoolSetting("Bool 1", "The first boolean setting.", false);
    NumberSetting mNumber1 = NumberSetting("Number 1", "The number setting.", 5.0f, 0.0f, 10.0f, 0.1f);
    BoolSetting mBool2 = BoolSetting("Bool 2", "The second boolean setting.", true);
    NumberSetting mNumber2 = NumberSetting("Number 2", "The second number setting.", 5.0f, 0.0f, 10.0f, 0.1f);
    NumberSetting mNumber21 = NumberSetting("Number 2.1", "The second number setting.", 5.0f, 0.0f, 10.0f, 0.1f);
    BoolSetting mBool3 = BoolSetting("Bool 3", "The second boolean setting.", true);
    NumberSetting mNumber3 = NumberSetting("Number 3", "The second number setting.", 5.0f, 0.0f, 10.0f, 0.1f);

    TestModule() : ModuleBase("TestModule", "A module for testing purposes", ModuleCategory::Misc, 0, false) {
        addSetting(&mMode);
        addSetting(&mBool1);
        addSetting(&mNumber1);
        VISIBILITY_CONDITION(mBool1, mMode.mValue == 0);
        VISIBILITY_CONDITION(mNumber1, mMode.mValue == 0);
        addSetting(&mBool2);
        addSetting(&mNumber2);
        addSetting(&mNumber21);
        VISIBILITY_CONDITION(mBool2, mMode.mValue == 1);
        VISIBILITY_CONDITION(mNumber2, mMode.mValue == 1);
        VISIBILITY_CONDITION(mNumber21, mMode.mValue == 1);
        addSetting(&mBool3);
        addSetting(&mNumber3);
        VISIBILITY_CONDITION(mBool3, mMode.mValue == 2);
        VISIBILITY_CONDITION(mNumber3, mMode.mValue == 2);
    }

    void onEnable() override;
    void onDisable() override;
    void onBaseTickEvent(class BaseTickEvent& event);
    void onRenderEvent(class RenderEvent& event);
};

REGISTER_MODULE(TestModule);