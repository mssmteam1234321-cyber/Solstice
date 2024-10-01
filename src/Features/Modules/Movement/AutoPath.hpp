#pragma once
//
// Created by vastrakai on 10/1/2024.
//


class AutoPath : public ModuleBase<AutoPath>
{
public:
    AutoPath() : ModuleBase("AutoPath", "Automatically pathfinds to a specified location", ModuleCategory::Movement, 0, false) {

    }

    void onEnable() override;
    void onDisable() override;
    void onBaseTickEvent(class BaseTickEvent& event);
};