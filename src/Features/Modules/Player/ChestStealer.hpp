#pragma once
//
// Created by vastrakai on 7/5/2024.
//

#include <Features/Modules/Module.hpp>


class ChestStealer : public ModuleBase<ChestStealer> {
public:
    BoolSetting mRandomizeDelay = BoolSetting("Randomize Delay", "Randomizes the delay between stealing items", false);
    NumberSetting mDelay = NumberSetting("Delay", "The delay between stealing items (in milliseconds)", 50, 0, 500, 1);
    NumberSetting mRandomizeMin = NumberSetting("Randomize Min", "The minimum delay to randomize", 50, 0, 500, 1);
    NumberSetting mRandomizeMax = NumberSetting("Randomize Max", "The maximum delay to randomize", 100, 0, 500, 1);
    BoolSetting mIgnoreUseless = BoolSetting("Ignore Useless", "Whether or not to ignore useless items", true);


    ChestStealer() : ModuleBase<ChestStealer>("ChestStealer", "Steals items from chests", ModuleCategory::Player, 0, false)
    {
        addSettings(&mRandomizeDelay, &mDelay, &mRandomizeMin, &mRandomizeMax, &mIgnoreUseless);
        VISIBILITY_CONDITION(mRandomizeMin, mRandomizeDelay.mValue == true);
        VISIBILITY_CONDITION(mRandomizeMax, mRandomizeDelay.mValue == true);
        VISIBILITY_CONDITION(mDelay, mRandomizeDelay.mValue == false);

        mNames = {
            {Lowercase, "cheststealer"},
            {LowercaseSpaced, "chest stealer"},
            {Normal, "ChestStealer"},
            {NormalSpaced, "Chest Stealer"}
        };
    }

    void onEnable() override;
    void onDisable() override;
    uint64_t getDelay() const;
    void onContainerScreenTickEvent(class ContainerScreenTickEvent& event) const;

    std::string getSettingDisplay() override {
        if (mRandomizeDelay.mValue)
        {
            return std::to_string(static_cast<int>(mRandomizeMin.mValue)) + " " + std::to_string(static_cast<int>(mRandomizeMax.mValue));
        }
        else
        {
            return std::to_string(static_cast<int>(mDelay.mValue)) + "";
        }
    }
};