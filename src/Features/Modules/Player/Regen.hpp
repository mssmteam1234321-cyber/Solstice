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
    EnumSettingT<CalcMode> mCalcMode = EnumSettingT<CalcMode>("Calc Mode", "The calculation mode destroy speed", CalcMode::Normal, "Normal", "Dynamic");
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
    BoolSetting mAntiSteal = BoolSetting("Anti Steal", "Stop mining if enemy tried to steal ore", false);
    BoolSetting mConfuse = BoolSetting("Confuse", "Confuse stealer", false);
    BoolSetting mAntiConfuse = BoolSetting("Anti Confuse", "Prevent stealer confused", false);
    BoolSetting mInfiniteDurability = BoolSetting("Infinite Durability", "Infinite durability for tools (may cause issues!)", false);
    BoolSetting mAlwaysMine = BoolSetting("Always mine", "Keep mining ore", false);
    BoolSetting mDebug = BoolSetting("Debug", "Send debug message in chat", false);
    BoolSetting mRenderBlock = BoolSetting("Render Block", "Renders the block you are currently breaking", true);
    BoolSetting mRenderProgressBar = BoolSetting("Render Progress Bar", "Renders the progress bar", true);

    Regen() : ModuleBase("Regen", "Automatically breaks redstone", ModuleCategory::Player, 0, false) {
        addSettings(&mMode, &mCalcMode, &mRange, &mDestroySpeed, &mOtherDestroySpeed, &mOldCalculation, &mSwing, &mHotbarOnly, &mUncover, &mQueueRedstone, &mSteal, &mAlwaysSteal, &mAntiSteal, &mConfuse, &mAntiConfuse, &mInfiniteDurability, &mAlwaysMine, &mDebug, &mRenderBlock, &mRenderProgressBar);

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

    struct DestroySpeedInfo {
        std::string blockName;
        float destroySpeed;
    };

    glm::ivec3 mCurrentBlockPos = { INT_MAX, INT_MAX, INT_MAX };
    glm::ivec3 mTargettingBlockPos = { INT_MAX, INT_MAX, INT_MAX };
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

    std::vector<glm::ivec3> mOffsetList = {
        glm::ivec3(0, -1, 0),
        glm::ivec3(0, 1, 0),
        glm::ivec3(0, 0, -1),
        glm::ivec3(0, 0, 1),
        glm::ivec3(-1, 0, 0),
        glm::ivec3(1, 0, 0),
    };

    /*
    61% hardened_clay
    61% green_concrete
    61% lime_terracotta
    61% brown_concrete
    61% sand
    61% dirt
    67% grass_block
    67% stone
    70% brown_powder
    54% sandstone
    54% sandstone_slab
    57% moss_block
    */


    // Dynamic Destroy Spped
    std::vector<DestroySpeedInfo> mDynamicSpeeds = { // make sure to include minecraft: before block names
        {"minecraft:hardened_clay", 0.61f},
        {"minecraft:green_concrete", 0.61f},
        {"minecraft:lime_terracotta", 0.61f},
        {"minecraft:brown_concrete", 0.61f},
        {"minecraft:sand", 0.61f},
        {"minecraft:dirt", 0.61f},
        {"minecraft:grass_block", 0.67f},
        {"minecraft:stone", 0.67f},
        {"minecraft:brown_powder", 0.70f},
        {"minecraft:sandstone", 0.54f},
        {"minecraft:sandstone_slab", 0.54f},
        {"minecraft:moss_block", 0.57f},
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
    void queueBlock(glm::ivec3 blockPos);
    bool isValidBlock(glm::ivec3 blockPos, bool redstoneOnly, bool exposedOnly, bool isStealing = false);
    PathFindingResult getBestPathToBlock(glm::ivec3 blockPos);

    std::string getSettingDisplay() override {
        return mMode.mValues[mMode.as<int>()];
    }

};