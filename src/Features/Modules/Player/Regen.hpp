#pragma once
#include <Features/Modules/Module.hpp>

class Regen : public ModuleBase<Regen> {
public:
    enum class Mode {
        Hive,
    };
    enum class CalcMode {
        Minecraft,
#ifdef __PRIVATE_BUILD__
        Test
#endif
    };
    enum class UncoverMode {
        Normal,
        Fast
    };
    enum class StealPriority {
        Mine,
        Steal
    };
    enum class ConfuseMode {
        Always,
        Auto
    };
    enum class ProgressBarStyle
    {
        Old,
        New
    };

    EnumSettingT<Mode> mMode = EnumSettingT<Mode>("Mode", "The regen mode", Mode::Hive, "Hive");
    EnumSettingT<CalcMode> mCalcMode = EnumSettingT<CalcMode>("Calc Mode", "The calculation mode destroy speed", CalcMode::Minecraft, "Minecraft"
#ifdef __PRIVATE_BUILD__
                                                              ,"Fast"
#endif
                                                              );
    NumberSetting mRange = NumberSetting("Range", "The max range for destroying blocks", 5, 0, 10, 0.01);
    NumberSetting mDestroySpeed = NumberSetting("Destroy Speed", "The destroy speed for Regen", 1, 0.01, 1, 0.01);
    NumberSetting mOtherDestroySpeed = NumberSetting("Other Destroy Speed", "The other destroy speed for Regen", 1, 0.01, 1, 0.01);
    BoolSetting mSwing = BoolSetting("Swing", "Swings when destroying blocks", false);
    BoolSetting mHotbarOnly = BoolSetting("Hotbar Only", "Only switch to tools in the hotbar", false);
    BoolSetting mUncover = BoolSetting("Uncover", "Uncover redstone if nothing around you is already exposed", false);
    EnumSettingT<UncoverMode> mUncoverMode = EnumSettingT<UncoverMode>("Uncover Mode", "The uncover mode", UncoverMode::Normal, "Normal", "Fast");
    NumberSetting mUncoverRange = NumberSetting("Uncover Range", "The max range for uncovering blocks", 3, 1, 8, 1);
    BoolSetting mQueueRedstone = BoolSetting("Queue Redstone", "Queue redstone blocks to break when max absorption is reached", false);
    BoolSetting mSteal = BoolSetting("Steal", "Steal the enemy's ore", false);
    EnumSettingT<StealPriority> mStealPriority = EnumSettingT<StealPriority>("Steal Priority", "Ore stealing priority", StealPriority::Mine, "Mine", "Steal");
    BoolSetting mAlwaysSteal = BoolSetting("Always Steal", "Steal the enemy's ore when max absorption is reached", false);
    BoolSetting mAntiSteal = BoolSetting("Anti Steal", "Stop mining if enemy tried to steal ore", false);
    BoolSetting mConfuse = BoolSetting("Confuse", "Confuse stealer", false);
    EnumSettingT<ConfuseMode> mConfuseMode = EnumSettingT<ConfuseMode>("Confuse Mode", "The mode for confuser", ConfuseMode::Always, "Always", "Auto");
    NumberSetting mConfuseDuration = NumberSetting("Confuse Duration", "The time for confuse", 3000, 1000, 10000, 500);
    BoolSetting mAntiConfuse = BoolSetting("Anti Confuse", "Dont steal if there are exposed redstones", false);
    BoolSetting mBlockOre = BoolSetting("Block Ore", "Cover opponent targetting ore", false);
    NumberSetting mBlockRange = NumberSetting("Block Range", "The max range for ore blocker", 5, 0, 10, 0.01);
    BoolSetting mMulti = BoolSetting("Multi", "Allows placing multiple blocks", false);
    BoolSetting mAntiCover = BoolSetting("Anti Cover", "Keep mining even if ore is covered", false);
    NumberSetting mCompensation = NumberSetting("Compensation", "The minium breaking progress percentage value for keep mining", 1, 0.01, 1, 0.01);
    BoolSetting mInfiniteDurability = BoolSetting("Infinite Durability", "Infinite durability for tools (may cause issues!)", false);
    BoolSetting mTest2 = BoolSetting("Prioritization", "test", false);
    BoolSetting mDynamicDestroySpeed = BoolSetting("Dynamic Destroy Speed", "use faster destroy speed to specified block", false);
    BoolSetting mOnGroundOnly = BoolSetting("OnGround Only", "use dynamic destroy speed only on ground", false);
    BoolSetting mNuke = BoolSetting("Nuke", "destroy block instantly", false);
    BoolSetting mAlwaysMine = BoolSetting("Always mine", "Keep mining ore", false);
    BoolSetting mDebug = BoolSetting("Debug", "Send debug message in chat", false);
    BoolSetting mConfuseNotify = BoolSetting("Confuse Notify", "Send message in chat when confused stealer", false);
    BoolSetting mBlockNotify = BoolSetting("Block Notify", "Send message in chat when you blocked ore/ore got covered", false);
    BoolSetting mFastOreNotify = BoolSetting("Fast Ore Notify", "Send message in chat when fast ore found", false);
    BoolSetting mSyncSpeedNotify = BoolSetting("Sync Speed Notify", "Send message in chat when broke block faster", false);
    BoolSetting mPriorityNotify = BoolSetting("Priority Notify", "Send message in chat when prioritized ore", false);
    BoolSetting mNukeNotify = BoolSetting("Nuke Notify", "Send message in chat when nuked block", false);
    BoolSetting mRenderBlock = BoolSetting("Render Block", "Renders the block you are currently breaking", true);
    EnumSettingT<ProgressBarStyle> mProgressBarStyle = EnumSettingT<ProgressBarStyle>("Progress Bar Style", "The render progress mode", ProgressBarStyle::New, "Old", "New");
    NumberSetting mOffset = NumberSetting("Offeset From Center", "render pos offset from center", 20, 0, 200, 0.1);
    BoolSetting mRenderProgressBar = BoolSetting("Render Progress Bar", "Renders the progress bar", true);
    BoolSetting mOreFaker = BoolSetting("Ore Faker", "Fakes targetting ore", false);
    BoolSetting mExposed = BoolSetting("Exposed", "Include exposed ore", false);
    BoolSetting mUnexposed = BoolSetting("Unexposed", "Include unexposed ore", false);
    BoolSetting mRenderFakeOre = BoolSetting("Render Fake Ore", "Renders the ore you are currenty faking", false);
    BoolSetting mReplace = BoolSetting("Replace", "Replaces enemy's ore with hardest block", false);
    BoolSetting mChecker = BoolSetting("Checker", "Checks if the block replacement was successful", false);
    BoolSetting mDynamicUncover = BoolSetting("Dynamic Uncover", "Disables uncover if enemy uncovering ores", false);
    NumberSetting mDisableDuration = NumberSetting("Disable Duration", "The time for dynamic uncover", 3, 1, 10, 1);
    BoolSetting mNoUncoverWhileStealing = BoolSetting("No Uncover While Stealing", "Disables uncover for 5 secs everytime u stole someone's ore", false);
    BoolSetting mStealerDetecter = BoolSetting("Stealer Detector", "Does some funnies if stealer detected :>", false);
    NumberSetting mAmountOfBlocksToDetect = NumberSetting("Stolen Blocks to Detect", "amount of blocks that should be stolen in past 5 seconds to detect stealer", 4, 1, 10, 1);
    BoolSetting mDisableUncover = BoolSetting("Disable Uncover", "Disables uncover for some seconds", false);
    NumberSetting mDisableSeconds = NumberSetting("Disable Duration", "amount of seconds uncover will be disabled for", 5, 1, 15, 1);
    BoolSetting mEnableAntiSteal = BoolSetting("Enable AntiSteal", "Enables anti-steal, disables if enemy didn't steal / didn't try to steal ur ore for past 5 seconds", false);

    Regen() : ModuleBase("Regen", "Automatically breaks redstone", ModuleCategory::Player, 0, false) {
        addSettings(
            &mProgressBarStyle,
            &mOffset,
            &mRenderProgressBar,
            &mRenderBlock,
            &mMode,
            &mCalcMode,
            &mRange,
            &mDestroySpeed,
            &mOtherDestroySpeed,
            &mSwing,
            &mHotbarOnly,
            &mUncover,
            &mUncoverMode,
            &mUncoverRange,
            &mQueueRedstone,
            &mSteal,
            &mStealPriority,
            &mAlwaysSteal,
            &mAntiSteal,
            &mConfuse,
            &mConfuseMode,
            &mConfuseDuration,
            &mAntiConfuse,
            &mBlockOre,
            &mBlockRange,
            &mMulti,
            &mAntiCover,
            &mCompensation,
            &mInfiniteDurability,
            &mTest2,
            &mDynamicDestroySpeed,
            &mOnGroundOnly,
            &mNuke,
            &mAlwaysMine,
            &mDebug,
            &mConfuseNotify,
            &mBlockNotify,
            &mFastOreNotify,
            &mSyncSpeedNotify,
            &mPriorityNotify,
            &mNukeNotify
        );
#ifdef __PRIVATE_BUILD__
        addSetting(&mOreFaker);
        addSettings(&mExposed, &mUnexposed);
        addSetting(&mRenderFakeOre);
        VISIBILITY_CONDITION(mExposed, mOreFaker.mValue);
        VISIBILITY_CONDITION(mUnexposed, mOreFaker.mValue);
        VISIBILITY_CONDITION(mRenderFakeOre, mOreFaker.mValue);
        addSettings(&mReplace, &mChecker);
        VISIBILITY_CONDITION(mReplace, mSteal.mValue);
        VISIBILITY_CONDITION(mChecker, mSteal.mValue && mReplace.mValue);
        addSetting(&mNoUncoverWhileStealing);
        addSettings(&mDynamicUncover, &mDisableDuration);
        VISIBILITY_CONDITION(mDynamicUncover, mUncover.mValue);
        VISIBILITY_CONDITION(mDisableDuration, mUncover.mValue && mDynamicUncover.mValue);
        addSettings(&mStealerDetecter, &mAmountOfBlocksToDetect);
        VISIBILITY_CONDITION(mAmountOfBlocksToDetect, mStealerDetecter.mValue);
        addSetting(&mDisableUncover);
        addSetting(&mDisableSeconds);
        addSetting(&mEnableAntiSteal);
        VISIBILITY_CONDITION(mDisableUncover, mStealerDetecter.mValue);
        VISIBILITY_CONDITION(mDisableSeconds, mDisableUncover.mValue && mStealerDetecter.mValue);
        VISIBILITY_CONDITION(mEnableAntiSteal, mStealerDetecter.mValue);
#endif

        VISIBILITY_CONDITION(mDestroySpeed, mCalcMode.mValue == CalcMode::Minecraft);
        VISIBILITY_CONDITION(mOtherDestroySpeed, mCalcMode.mValue == CalcMode::Minecraft);

        VISIBILITY_CONDITION(mUncoverMode, mUncover.mValue);
        VISIBILITY_CONDITION(mUncoverRange, mUncover.mValue && mUncoverMode.mValue == UncoverMode::Normal);

        VISIBILITY_CONDITION(mStealPriority, mSteal.mValue);
        VISIBILITY_CONDITION(mAlwaysSteal, mSteal.mValue);

        VISIBILITY_CONDITION(mConfuseMode, mConfuse.mValue);
        VISIBILITY_CONDITION(mConfuseDuration, mConfuse.mValue && mConfuseMode.mValue == ConfuseMode::Auto);

        VISIBILITY_CONDITION(mBlockRange, mBlockOre.mValue);
        VISIBILITY_CONDITION(mMulti, mBlockOre.mValue);

        VISIBILITY_CONDITION(mCompensation, mAntiCover.mValue);

        VISIBILITY_CONDITION(mOnGroundOnly, mDynamicDestroySpeed.mValue);
        VISIBILITY_CONDITION(mNuke, mDynamicDestroySpeed.mValue && mOnGroundOnly.mValue);

        // Debug
        VISIBILITY_CONDITION(mConfuseNotify, mDebug.mValue);
        VISIBILITY_CONDITION(mBlockNotify, mDebug.mValue);
        VISIBILITY_CONDITION(mFastOreNotify, mDebug.mValue);
        VISIBILITY_CONDITION(mSyncSpeedNotify, mDebug.mValue);
        VISIBILITY_CONDITION(mPriorityNotify, mDebug.mValue);

        VISIBILITY_CONDITION(mOffset, mProgressBarStyle.mValue == ProgressBarStyle::New);

        mNames = {
            {Lowercase, "regen"},
            {LowercaseSpaced, "regen"},
            {Normal, "Regen"},
            {NormalSpaced, "Regen"}
        };

        gFeatureManager->mDispatcher->listen<RenderEvent, &Regen::onRenderEvent, nes::event_priority::LAST>(this);
    }

    bool stealEnabled = false;

    uint64_t lastStoleTime = 0;
    bool antiStealerEnabled = false;
    bool stealerDetected = false;
    int amountOfStolenBlocks = 0;
    uint64_t stealerDetectionStartTime = 0;
    bool startedStealerDetection = false;
    uint64_t uncoverDisabledTime = 0;
    bool uncoverEnabled = true;
    uint64_t lastStealerDetected = 0;

    struct PathFindingResult {
        glm::ivec3 blockPos;
        float time;
    };

    static inline glm::ivec3 mCurrentBlockPos = { INT_MAX, INT_MAX, INT_MAX };
    glm::ivec3 mTargettingBlockPos = { INT_MAX, INT_MAX, INT_MAX };
    glm::ivec3 mLastTargettingBlockPos = { INT_MAX, INT_MAX, INT_MAX };
    glm::ivec3 mEnemyTargettingBlockPos = { INT_MAX, INT_MAX, INT_MAX };
    glm::ivec3 mLastEnemyLayerBlockPos = { INT_MAX, INT_MAX, INT_MAX };
    bool mCanSteal = false;
    bool mIsStealing = false;
    bool mCurrentUncover = false;
    int mCurrentBlockFace = -1;
    float mBreakingProgress = 0.f;
    float mCurrentDestroySpeed = 1.f;
    static inline bool mIsMiningBlock = false;
    static inline bool mWasMiningBlock = false;
    bool mIsUncovering = false;
    bool mWasUncovering = false;
    float mLastTargettingBlockPosDestroySpeed = 1.f;
    int mLastToolSlot = 0;
    bool mIsConfuserActivated = false;
    glm::ivec3 mLastConfusedPos = { INT_MAX, INT_MAX, INT_MAX };
    bool mOffGround = false;

    bool mShouldRotate = false;
    bool mShouldSpoofSlot = false;
    bool mShouldSetbackSlot = false;
    glm::ivec3 mBlackListedOrePos = { INT_MAX, INT_MAX, INT_MAX };
    int mPreviousSlot = -1;
    int mToolSlot = -1;

    std::vector<glm::ivec3> miningRedstones;
    glm::ivec3 mCurrentPlacePos = { INT_MAX, INT_MAX, INT_MAX };

    uint64_t mLastBlockPlace = 0;
    uint64_t mLastStealerUpdate = 0;
    uint64_t mLastStealerDetected = 0;
    uint64_t mLastConfuse = 0;
    uint64_t mLastUncoverDetected = 0;
    uint64_t mLastReplaced = 0;
    int mLastPlacedBlockSlot = 0;

    bool mStoleOreLastTime = false;
    glm::ivec3 mLastReplacedPos = { INT_MAX, INT_MAX, INT_MAX };

    std::vector<glm::ivec3> mFakePositions;
    //std::vector<glm::ivec3> mLastUpdatedBlockPositions;
    std::vector<glm::ivec3> mLastBrokenOrePos;
    std::vector<glm::ivec3> mLastBrokenCoveringBlockPos;

    std::vector<glm::ivec3> mOffsetList = {
        glm::ivec3(0, -1, 0),
        glm::ivec3(0, 1, 0),
        glm::ivec3(0, 0, -1),
        glm::ivec3(0, 0, 1),
        glm::ivec3(-1, 0, 0),
        glm::ivec3(1, 0, 0),
    };

    void onEnable() override;
    void onDisable() override;
    void onBaseTickEvent(class BaseTickEvent& event);
    void onRenderEvent(class RenderEvent& event);
    void renderProgressBar();
    void renderNewProgressBar();
    void renderBlock();
    void renderFakeOres();
    void onPacketOutEvent(class PacketOutEvent& event);
    void onPacketInEvent(class PacketInEvent& event);
    void initializeRegen();
    void resetSyncSpeed();
    void queueBlock(glm::ivec3 blockPos);
    bool isValidBlock(glm::ivec3 blockPos, bool redstoneOnly, bool exposedOnly, bool isStealing = false);
    bool isValidRedstone(glm::ivec3 blockPos);
    PathFindingResult getBestPathToBlock(glm::ivec3 blockPos);

    std::string getSettingDisplay() override {
        return mMode.mValues[mMode.as<int>()];
    }

};