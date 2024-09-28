#pragma once
//
// Created by vastrakai on 8/9/2024.
//

class Disabler : public ModuleBase<Disabler> {
public:
    enum class Mode {
        Flareon,
        Sentinel,
        Custom
    };
    enum class DisablerType {
        PingSpoof,
        PingHolder,
#ifdef __PRIVATE_BUILD__
        MoveFix
#endif
    };

    EnumSettingT<Mode> mMode = EnumSettingT<Mode>("Mode", "The mode to use for the disabler.", Mode::Flareon, "Flareon", "Sentinel", "Custom");
    EnumSettingT<DisablerType> mDisablerType = EnumSettingT<DisablerType>("Disabler Type", "The type of disabler to use.", DisablerType::PingSpoof, "Ping Spoof", "Ping Holder"
#ifdef __PRIVATE_BUILD__
        ,"Move Fix"
#endif
        );
    BoolSetting mRandomizeDelay = BoolSetting("Randomize Delay", "Whether or not to randomize the delay", true);
    NumberSetting mDelay = NumberSetting("Delay", "The delay to use for the disabler", 10000, 0, 100000, 1);
    NumberSetting mMinDelay = NumberSetting("Min Delay", "The minimum delay to randomize", 6000, 0, 100000, 1);
    NumberSetting mMaxDelay = NumberSetting("Max Delay", "The maximum delay to randomize", 10000, 0, 100000, 1);
    BoolSetting mInteract = BoolSetting("Interact", "Whether or not to send interact packet before attack", false);
    BoolSetting mCancel = BoolSetting("Cancel", "Whether or not to cancel networkstacklatency packet", false);
    BoolSetting mGlide = BoolSetting("Glide", "Whether or not to send start gliding packet", false);
    BoolSetting mOnGroundSpoof = BoolSetting("On Ground Spoof", "Whether or not to spoof on ground", false);
    BoolSetting mInputSpoof = BoolSetting("Input Spoof", "Whether or not to spoof input mode", false);
    BoolSetting mClickPosFix = BoolSetting("Click Pos Fix", "Whether or not to fix click pos", false);

    Disabler() : ModuleBase<Disabler>("Disabler", "Attempts to disable Anti-Cheat checks by exploiting them.", ModuleCategory::Misc, 0, false){
        addSetting(&mMode);
        addSetting(&mDisablerType);
        addSetting(&mRandomizeDelay);
        addSetting(&mDelay);
        addSetting(&mMinDelay);
        addSetting(&mMaxDelay);
        addSettings(&mInteract, &mCancel, &mGlide, &mOnGroundSpoof, &mInputSpoof, &mClickPosFix);

        VISIBILITY_CONDITION(mDisablerType, mMode.mValue == Mode::Flareon);
        VISIBILITY_CONDITION(mRandomizeDelay, mMode.mValue == Mode::Flareon && mDisablerType.mValue == DisablerType::PingSpoof);

        VISIBILITY_CONDITION(mDelay, mMode.mValue == Mode::Flareon && mDisablerType.mValue == DisablerType::PingSpoof && !mRandomizeDelay.mValue);
        VISIBILITY_CONDITION(mMinDelay, mMode.mValue == Mode::Flareon && mDisablerType.mValue == DisablerType::PingSpoof && mRandomizeDelay.mValue);
        VISIBILITY_CONDITION(mMaxDelay, mMode.mValue == Mode::Flareon && mDisablerType.mValue == DisablerType::PingSpoof && mRandomizeDelay.mValue);

        VISIBILITY_CONDITION(mInteract, mMode.mValue == Mode::Custom);
        VISIBILITY_CONDITION(mCancel, mMode.mValue == Mode::Custom);
        VISIBILITY_CONDITION(mGlide, mMode.mValue == Mode::Custom);
        VISIBILITY_CONDITION(mOnGroundSpoof, mMode.mValue == Mode::Custom);
        VISIBILITY_CONDITION(mInputSpoof, mMode.mValue == Mode::Custom);
        VISIBILITY_CONDITION(mClickPosFix, mMode.mValue == Mode::Custom);


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
    Actor* mFirstAttackedActor = nullptr;

    void onEnable() override;
    void onDisable() override;
    void onPacketOutEvent(class PacketOutEvent& event);
    void onRunUpdateCycleEvent(class RunUpdateCycleEvent& event);
    int64_t getDelay() const;
    void onPingUpdateEvent(class PingUpdateEvent& event);
    void onSendImmediateEvent(class SendImmediateEvent& event);
};