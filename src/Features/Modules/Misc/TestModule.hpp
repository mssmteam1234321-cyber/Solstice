//
// Created by vastrakai on 6/28/2024.
//
#pragma once

#include <Features/FeatureManager.hpp>
#include <Features/Modules/Module.hpp>
#include <Features/Modules/ModuleManager.hpp>
#include <spdlog/spdlog.h>
#include <Solstice.hpp>

class TestModule : public ModuleBase<TestModule> {
public:
    TestModule() : ModuleBase("TestModule", "A module for testing purposes", ModuleCategory::Misc, 0, false) {
        // Register your features here
    }

    void onEnable() override;
    void onDisable() override;
};

REGISTER_MODULE(TestModule);