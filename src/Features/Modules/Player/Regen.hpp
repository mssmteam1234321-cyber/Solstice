#pragma once
#include <Features/Modules/Module.hpp>

class Regen : public ModuleBase<Regen> {
public:
    enum class Mode {
        Flareon,
    };
    enum class CalcMode {
        Normal,
        Dynamic
    };
    enum class UncoverMode {
        Normal,
        Fast
    };
    enum class StealPriority {
        Mine,
        Steal
    };
    EnumSettingT<Mode> mMode = EnumSettingT<Mode>("Mode", "The regen mode", Mode::Flareon, "Flareon");
    EnumSettingT<CalcMode> mCalcMode = EnumSettingT<CalcMode>("Calc Mode", "The calculation mode destroy speed", CalcMode::Normal, "Normal", "Dynamic");
    NumberSetting mRange = NumberSetting("Range", "The max range for destroying blocks", 5, 0, 10, 0.01);
    NumberSetting mDestroySpeed = NumberSetting("Destroy Speed", "The destroy speed for Regen", 1, 0.01, 1, 0.01);
    NumberSetting mOtherDestroySpeed = NumberSetting("Other Destroy Speed", "The other destroy speed for Regen", 1, 0.01, 1, 0.01);
    BoolSetting mOldCalculation = BoolSetting("Old Calculation", "Use the old calculation for breaking blocks", false);
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
    BoolSetting mAntiConfuse = BoolSetting("Anti Confuse", "Dont steal if there are exposed redstones", false);
    BoolSetting mBlockOre = BoolSetting("Block Ore", "Cover opponent targetting ore", false);
    BoolSetting mAntiCover = BoolSetting("Anti Cover", "Keep mining even if ore is covered", false);
    NumberSetting mCompensation = NumberSetting("Compensation", "The minium breaking progress percentage value for keep mining", 1, 0.01, 1, 0.01);
    BoolSetting mInfiniteDurability = BoolSetting("Infinite Durability", "Infinite durability for tools (may cause issues!)", false);
    BoolSetting mTest = BoolSetting("Test", "test", false);
    BoolSetting mAlwaysMine = BoolSetting("Always mine", "Keep mining ore", false);
    BoolSetting mDebug = BoolSetting("Debug", "Send debug message in chat", false);
    BoolSetting mStealNotify = BoolSetting("Steal Notify", "Send message in chat when stole/stolen ore", false);
    BoolSetting mCoverNotify = BoolSetting("Cover Notify", "Send message in chat when you covered ore", false);
    BoolSetting mFastOreNotify = BoolSetting("Fast Ore Notify", "Send message in chat when fast ore found", false);
    BoolSetting mSyncSpeedNotify = BoolSetting("Sync Speed Notify", "Send message in chat when broke block faster", false);
    BoolSetting mRenderBlock = BoolSetting("Render Block", "Renders the block you are currently breaking", true);
    BoolSetting mRenderProgressBar = BoolSetting("Render Progress Bar", "Renders the progress bar", true);

    Regen() : ModuleBase("Regen", "Automatically breaks redstone", ModuleCategory::Player, 0, false) {
        addSettings(&mMode, &mCalcMode, &mRange, &mDestroySpeed, &mOtherDestroySpeed, &mOldCalculation, &mSwing, &mHotbarOnly, &mUncover, &mUncoverMode, &mUncoverRange, &mQueueRedstone, &mSteal, &mStealPriority, &mAlwaysSteal, &mAntiSteal, &mConfuse, &mAntiConfuse, &mBlockOre, &mAntiCover, &mCompensation, &mInfiniteDurability, &mTest, &mAlwaysMine, &mDebug, &mStealNotify, &mCoverNotify, &mFastOreNotify, &mSyncSpeedNotify, &mRenderBlock, &mRenderProgressBar);

        VISIBILITY_CONDITION(mUncoverMode, mUncover.mValue);
        VISIBILITY_CONDITION(mUncoverRange, mUncover.mValue && mUncoverMode.mValue == UncoverMode::Normal);

        VISIBILITY_CONDITION(mStealPriority, mSteal.mValue);

        VISIBILITY_CONDITION(mCompensation, mAntiCover.mValue);

        VISIBILITY_CONDITION(mTest, mInfiniteDurability.mValue);

        // Debug
        VISIBILITY_CONDITION(mStealNotify, mDebug.mValue);
        VISIBILITY_CONDITION(mCoverNotify, mDebug.mValue);
        VISIBILITY_CONDITION(mFastOreNotify, mDebug.mValue);
        VISIBILITY_CONDITION(mSyncSpeedNotify, mDebug.mValue);

        mNames = {
            {Lowercase, "regen"},
            {LowercaseSpaced, "regen"},
            {Normal, "Regen"},
            {NormalSpaced, "Regen"}
        };

        gFeatureManager->mDispatcher->listen<RenderEvent, &Regen::onRenderEvent, nes::event_priority::LAST>(this);
    }

    struct PathFindingResult {
        glm::ivec3 blockPos;
        float time;
    };

    glm::ivec3 mCurrentBlockPos = { INT_MAX, INT_MAX, INT_MAX };
    glm::ivec3 mTargettingBlockPos = { INT_MAX, INT_MAX, INT_MAX };
    glm::ivec3 mLastTargettingBlockPos = { INT_MAX, INT_MAX, INT_MAX };
    glm::ivec3 mEnemyTargettingBlockPos = { INT_MAX, INT_MAX, INT_MAX };
    glm::ivec3 mLastEnemyLayerBlockPos = { INT_MAX, INT_MAX, INT_MAX };
    bool mCanSteal = false;
    bool mIsStealing = false;
    int mCurrentBlockFace = -1;
    float mBreakingProgress = 0.f;
    float mCurrentDestroySpeed = 1.f;
    bool mIsMiningBlock = false;
    bool mWasMiningBlock = false;
    bool mIsUncovering = false;
    bool mWasUncovering = false;
    float mLastTargettingBlockPosDestroySpeed = 1.f;
    int mLastToolSlot = 0;
    bool mIsConfuserActivated = false;
    glm::ivec3 mLastConfusedPos = { INT_MAX, INT_MAX, INT_MAX };
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
    int mLastPlacedBlockSlot = 0;

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
    void renderBlock();
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