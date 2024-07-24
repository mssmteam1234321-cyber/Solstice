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
    EnumSettingT<Mode> mMode = EnumSettingT<Mode>("Mode", "The regen mode", Mode::Flareon, "Flareon");
    EnumSettingT<CalcMode> mCalcMode = EnumSettingT<CalcMode>("CalcMode", "The calculation mode destroy speed", CalcMode::Normal, "Normal", "Dynamic");
    NumberSetting mRange = NumberSetting("Range", "The max range for destroying blocks", 5, 0, 10, 0.01);
    NumberSetting mDestroySpeed = NumberSetting("Destroy Speed", "The destroy speed for Regen", 1, 0.01, 1, 0.01);
    NumberSetting mOtherDestroySpeed = NumberSetting("Other Destroy Speed", "The other destroy speed for Regen", 1, 0.01, 1, 0.01);
    BoolSetting mOldCalculation = BoolSetting("Old Calculation", "Use the old calculation for breaking blocks", false);
    BoolSetting mSwing = BoolSetting("Swing", "Swings when destroying blocks", false);
    BoolSetting mHotbarOnly = BoolSetting("Hotbar Only", "Only switch to tools in the hotbar", false);
    BoolSetting mUncover = BoolSetting("Uncover", "Uncover redstone if nothing around you is already exposed", false);
    BoolSetting mQueueRedstone = BoolSetting("Queue Redstone", "Queue redstone blocks to break when max absorption is reached", false);
    BoolSetting mSteal = BoolSetting("Steal", "Steal the enemy's ore", false);
    BoolSetting mAlwaysSteal = BoolSetting("Always Steal", "Steal the enemy's ore when max absorption is reached", false);
    BoolSetting mAntiSteal = BoolSetting("AntiSteal", "Stop mining if enemy tried to steal ore", false);
    BoolSetting mConfuse = BoolSetting("Confuse", "Confuse stealer", false);
    BoolSetting mAlwaysMine = BoolSetting("Always mine", "Keep mining ore", false);
    BoolSetting mRenderBlock = BoolSetting("Render Block", "Renders the block you are currently breaking", true);

    Regen() : ModuleBase("Regen", "Automatically breaks redstone", ModuleCategory::Player, 0, false) {
        addSettings(&mMode, &mCalcMode, &mRange, &mDestroySpeed, &mOtherDestroySpeed, &mOldCalculation, &mSwing, &mHotbarOnly, &mUncover, &mQueueRedstone, &mSteal, &mAlwaysSteal, &mAntiSteal, &mConfuse, &mAlwaysMine, &mRenderBlock);

        mNames = {
            {Lowercase, "regen"},
            {LowercaseSpaced, "regen"},
            {Normal, "Regen"},
            {NormalSpaced, "Regen"}
        };
    }

    struct PathFindingResult {
        glm::ivec3 blockPos;
        float time;
    };

    struct destroySpeedInfo {
        std::string blockName;
        float destroySpeed;
    };

    glm::ivec3 mCurrentBlockPos = { 0, 0, 0 };
    glm::ivec3 mTargettingBlockPos = { 0, 0, 0 };
    glm::ivec3 mEnemyTargettingBlockPos = { 0, 0, 0 };
    glm::ivec3 mLastEnemyLayerBlockPos = { 0, 0, 0 };
    bool mCanSteal = false;
    bool mIsStealing = false;
    int mCurrentBlockFace = -1;
    float mBreakingProgress = 0.f;
    float mCurrentDestroySpeed = 1.f;
    bool mIsMiningBlock = false;
    bool mIsUncovering = false;
    bool mIsConfuserActivated = false;
    glm::ivec3 mLastConfusedPos = { INT_MAX, INT_MAX, INT_MAX };
    bool mShouldRotate = false;
    bool mShouldSpoofSlot = false;
    bool mShouldSetbackSlot = false;
    glm::ivec3 mBlackListedOrePos = { INT_MAX, INT_MAX, INT_MAX };
    int mPreviousSlot = -1;
    int mToolSlot = -1;

    uint64_t mLastBlockPlace = 0;
    uint64_t mLastStealerUpdate = 0;
    int mLastPlacedBlockSlot = 0;

    std::vector<glm::ivec3> offsetList = {
        glm::ivec3(0, -1, 0),
        glm::ivec3(0, 1, 0),
        glm::ivec3(0, 0, -1),
        glm::ivec3(0, 0, 1),
        glm::ivec3(-1, 0, 0),
        glm::ivec3(1, 0, 0),
    };

    // Dynamic Destroy Spped
    std::vector<destroySpeedInfo> dynamicSpeeds = {
        { "minecraft:moss_block", 0.57 },
        { "minecraft:hardened_clay", 0.71 },
        { "minecraft:brown_powder", 0.7 },
        { "minecraft:brown_concrete", 0.7 },
        { "minecraft:green_concrete", 0.7 },
        { "minecraft:lime_concrete", 0.7 },
        { "minecraft:sandstone", 0.57 },
        { "minecraft:sandstone_slab", 0.57 },

    };

    void onEnable() override;
    void onDisable() override;
    void onBaseTickEvent(class BaseTickEvent& event);
    void onRenderEvent(class RenderEvent& event);
    void onPacketOutEvent(class PacketOutEvent& event);
    void onPacketInEvent(class PacketInEvent& event);
    void initializeRegen();
    void queueBlock(glm::ivec3 blockPos);
    bool isValidBlock(glm::ivec3 blockPos, bool redstoneOnly, bool exposedOnly, bool isStealing = false);
    PathFindingResult getBestPathToBlock(glm::ivec3 blockPos);

    std::string getSettingDisplay() override {
        return mMode.mValues[mMode.as<int>()];
    }

};