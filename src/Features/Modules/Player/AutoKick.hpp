#pragma once
//
// 8/22/2024.
//

#include <Features/Modules/Module.hpp>

class AutoKick : public ModuleBase<AutoKick> {
public:
    enum class Mode {
        Push,
    };

    EnumSettingT<Mode> mMode = EnumSettingT<Mode>("Mode", "The mode for auto kick", Mode::Push, "Push");
    NumberSetting mDelay = NumberSetting("Delay", "The delay in ms to place block", 1000, 100, 5000, 10);
    BoolSetting mHotbarOnly = BoolSetting("Hotbar Only", "Only switch to boomboxes in the hotbar", true);
    BoolSetting mOnGroundOnly = BoolSetting("OnGround Only", "Place block while target is onground", true);
    NumberSetting mMaxOnGroundTicks = NumberSetting("OnGround Ticks", "The max onground ticks to place block", 10, 0, 100, 5);
    NumberSetting mOpponentPing = NumberSetting("Opponent Ping", "Specify opponent ping time", 90, 0, 300, 10);
    AutoKick() : ModuleBase("AutoKick", "Automatically kicks player in the hive", ModuleCategory::Player, 0, false)
    {
        addSetting(&mMode);
        addSetting(&mDelay);
        addSetting(&mHotbarOnly);
        addSetting(&mOnGroundOnly);
        addSetting(&mMaxOnGroundTicks);
        addSetting(&mOpponentPing);

        VISIBILITY_CONDITION(mMaxOnGroundTicks, mOnGroundOnly.mValue);

        mNames = {
              {Lowercase, "autokick"},
                {LowercaseSpaced, "auto kick"},
                {Normal, "AutoKick"},
                {NormalSpaced, "Auto Kick"}
        };
    };

    int mPreviousSlot = 0;
    bool mSelectedSlot = false;
    bool mShouldRotate = false;

    glm::vec3 mLastTargetPos = { INT_MAX, INT_MAX, INT_MAX };
    bool mUsePrediction = false;
    int mOnGroundTicks = 0;

    uint64_t mLastBlockPlace = 0;
    glm::ivec3 mCurrentPlacePos = { INT_MAX, INT_MAX, INT_MAX };

    uint64_t mPing = 100;
    uint64_t mEventDelay = 0;

    void onEnable();
    void onDisable() override;
    void onBaseTickEvent(class BaseTickEvent& event);
    void onPacketOutEvent(class PacketOutEvent& event);
    void onSendImmediateEvent(class SendImmediateEvent& event);
    void onPingUpdateEvent(class PingUpdateEvent& event);
};