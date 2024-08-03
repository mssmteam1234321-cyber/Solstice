#pragma once
//
// Created by vastrakai on 8/3/2024.
//


class NetSkip : public ModuleBase<NetSkip>
{
public:
    BoolSetting mRandomizeDelay = BoolSetting("Randomize Delay", "Randomizes the delay", false);
    NumberSetting mDelay = NumberSetting("Delay", "The delay in ms to skip packets", 101, 0, 1000, 1);
    NumberSetting mRandomizeMin = NumberSetting("Randomize Min", "The minimum delay to randomize", 0, 0, 1000, 1);
    NumberSetting mRandomizeMax = NumberSetting("Randomize Max", "The maximum delay to randomize", 100, 0, 1000, 1);
    NetSkip() : ModuleBase<NetSkip>("NetSkip", "Skips packets", ModuleCategory::Misc, 0, false) {
        addSettings(
            &mRandomizeDelay,
            &mDelay,
            &mRandomizeMin,
            &mRandomizeMax
        );

        VISIBILITY_CONDITION(mDelay, !mRandomizeDelay.mValue);
        VISIBILITY_CONDITION(mRandomizeMin, mRandomizeDelay.mValue);
        VISIBILITY_CONDITION(mRandomizeMax, mRandomizeDelay.mValue);


        mNames = {
            {Lowercase, "netskip"},
            {LowercaseSpaced, "net skip"},
            {Normal, "NetSkip"},
            {NormalSpaced, "Net Skip"}
        };
    }

    [[nodiscard]] int64_t getDelay() const;
    void onEnable() override;
    void onDisable() override;
    void onRunUpdateCycleEvent(class RunUpdateCycleEvent& event);
};