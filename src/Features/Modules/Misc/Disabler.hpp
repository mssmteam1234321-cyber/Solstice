#pragma once
//
// Created by vastrakai on 8/9/2024.
//

class Disabler : public ModuleBase<Disabler> {
public:
    enum class Mode {
        FlareonOld,
        Sentinel
    };
    enum class DisablerType {
        PingSpoof,
        PingHolder
    };

    EnumSettingT<Mode> mMode = EnumSettingT<Mode>("Mode", "The mode to use for the disabler.", Mode::FlareonOld, "FlareonOld", "Sentinel");
    EnumSettingT<DisablerType> mDisablerType = EnumSettingT<DisablerType>("Disabler Type", "The type of disabler to use.", DisablerType::PingSpoof, "Ping Spoof", "Ping Holder");
    BoolSetting mRandomizeDelay = BoolSetting("Randomize Delay", "Whether or not to randomize the delay", true);
    NumberSetting mDelay = NumberSetting("Delay", "The delay to use for the disabler", 10000, 0, 100000, 1);
    NumberSetting mMinDelay = NumberSetting("Min Delay", "The minimum delay to randomize", 6000, 0, 100000, 1);
    NumberSetting mMaxDelay = NumberSetting("Max Delay", "The maximum delay to randomize", 10000, 0, 100000, 1);

    Disabler() : ModuleBase<Disabler>("Disabler", "Attempts to disable Anti-Cheat checks by exploiting them.", ModuleCategory::Misc, 0, false){
        addSetting(&mMode);
        addSetting(&mDisablerType);
        addSetting(&mRandomizeDelay);
        addSetting(&mDelay);
        addSetting(&mMinDelay);
        addSetting(&mMaxDelay);

        VISIBILITY_CONDITION(mDisablerType, mMode.mValue == Mode::FlareonOld);
        VISIBILITY_CONDITION(mRandomizeDelay, mDisablerType.mValue == DisablerType::PingSpoof);

        VISIBILITY_CONDITION(mDelay, mMode.mValue == Mode::FlareonOld && mDisablerType.mValue == DisablerType::PingSpoof && !mRandomizeDelay.mValue);
        VISIBILITY_CONDITION(mMinDelay, mMode.mValue == Mode::FlareonOld && mDisablerType.mValue == DisablerType::PingSpoof && mRandomizeDelay.mValue);
        VISIBILITY_CONDITION(mMaxDelay, mMode.mValue == Mode::FlareonOld && mDisablerType.mValue == DisablerType::PingSpoof && mRandomizeDelay.mValue);


        mNames = {
            {Lowercase, "disabler"},
            {LowercaseSpaced, "disabler"},
            {Normal, "Disabler"},
            {NormalSpaced, "Disabler"}
        };
    }

    uint64_t mClientTicks = 0;
    bool mShouldUpdateClientTicks = false;
    glm::vec3 mLastPosition = { 0, 0, 0 };

    void onEnable() override;
    void onDisable() override;
    void onPacketOutEvent(class PacketOutEvent& event);
    void onRunUpdateCycleEvent(class RunUpdateCycleEvent& event);
    int64_t getDelay() const;
    void onPingUpdateEvent(class PingUpdateEvent& event);
    void onSendImmediateEvent(class SendImmediateEvent& event);
};