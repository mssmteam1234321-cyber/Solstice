#pragma once
//
// Created by vastrakai on 7/12/2024.
//

#include <Features/Modules/Module.hpp>

class AutoTool : public ModuleBase<AutoTool> {
public:
    BoolSetting mHotbarOnly = BoolSetting{ "HotbarOnly", "Only switch to tools in the hotbar", false };
    AutoTool() : ModuleBase("AutoTool", "Automatically switches to the fastest tool for the block you're mining", ModuleCategory::Player, 0, false)
    {
        addSetting(&mHotbarOnly);

        mNames = {
              {Lowercase, "autotool"},
                {LowercaseSpaced, "auto tool"},
                {Normal, "AutoTool"},
                {NormalSpaced, "Auto Tool"}
        };
    };

    int mOldSlot = -1;

    void onEnable();
    void onDisable() override;
    void onBaseTickEvent(class BaseTickEvent& event);
};