#pragma once
//
// Created by vastrakai on 8/3/2024.
//


class Step : public ModuleBase<Step>
{
public:
    NumberSetting mStepHeight = NumberSetting("Step Height", "The max height to step up blocks", 0.50f, 0.0f, 4.f, 0.1f);

    Step() : ModuleBase<Step>("Step", "Automatically steps up blocks", ModuleCategory::Movement, 0, false) {
        addSetting(&mStepHeight);

        mNames = {
            {Lowercase, "step"},
            {LowercaseSpaced, "step"},
            {Normal, "Step"},
            {NormalSpaced, "Step"}
        };
    }

    void onEnable() override;
    void onDisable() override;
    void onBaseTickEvent(class BaseTickEvent& event);
};