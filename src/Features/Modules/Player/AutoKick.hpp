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
    NumberSetting mRange = NumberSetting("Range", "The range at which to push entities", 5, 3, 8, 0.1);
    NumberSetting mDelay = NumberSetting("Delay", "The delay in ms to place block", 1000, 100, 5000, 10);
    BoolSetting mHotbarOnly = BoolSetting("Hotbar Only", "Only switch to boomboxes in the hotbar", true);
    BoolSetting mAllowDiagonal = BoolSetting("Allow Diagonal", "Allow diagonal prediction", true);
    BoolSetting mOnGroundOnly = BoolSetting("OnGround Only", "Place block while target is onground", true);
    NumberSetting mMaxOnGroundTicks = NumberSetting("OnGround Ticks", "The max onground ticks to place block", 10, 0, 100, 5);
    NumberSetting mOpponentPing = NumberSetting("Opponent Ping", "Specify opponent ping time", 90, 0, 300, 10);
    BoolSetting mSpamPlace = BoolSetting("Spam Place", "Reset delay if block ghosted", false);
    BoolSetting mDebug = BoolSetting("Debug", "Send debug message in chat", false);
    AutoKick() : ModuleBase("AutoKick", "Automatically kicks player in the hive", ModuleCategory::Player, 0, false)
    {
        addSetting(&mMode);
        addSetting(&mRange);
        addSetting(&mDelay);
        addSetting(&mHotbarOnly);
        addSetting(&mAllowDiagonal);
        addSetting(&mOnGroundOnly);
        addSetting(&mMaxOnGroundTicks);
        addSetting(&mOpponentPing);
        addSetting(&mSpamPlace);
        addSetting(&mDebug);

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
    uint64_t mLastBlockUpdated = 0;
    glm::ivec3 mLastServerBlockPos = { INT_MAX, INT_MAX, INT_MAX };
    bool mFlagged = false;

    uint64_t mPing = 100;
    uint64_t mEventDelay = 0;

    std::vector<glm::ivec3> mRecentlyUpdatedBlockPositions;

    std::map<std::string, int> mFlagCounter;

    void onEnable();
    void onDisable() override;
    void onBaseTickEvent(class BaseTickEvent& event);
    void onPacketOutEvent(class PacketOutEvent& event);
    void onPacketInEvent(class PacketInEvent& event);
    void onSendImmediateEvent(class SendImmediateEvent& event);
    void onPingUpdateEvent(class PingUpdateEvent& event);
};