// 7/28/2024
#pragma once
#include <Features/Modules/Module.hpp>

class OreMiner : public ModuleBase<OreMiner> {
public:
    enum class UncoverMode {
        None,
        PathFind,
        UnderGround,
    };
    EnumSettingT<UncoverMode> mUncoverMode = EnumSettingT<UncoverMode>("Uncover Mode", "The mode for uncover", UncoverMode::None, "None", "Path Find", "Under Ground");
    NumberSetting mRange = NumberSetting("Range", "The max range for destroying blocks", 5, 0, 10, 0.01);
    NumberSetting mUncoverRange = NumberSetting("Uncover Range", "The max range for uncovering blocks", 3, 1, 8, 1);
    NumberSetting mDestroySpeed = NumberSetting("Destroy Speed", "The destroy speed for Ore miner", 1, 0.01, 1, 0.01);
    BoolSetting mEmerald = BoolSetting("Emerald", "Destroy emerald ore", false);
    BoolSetting mDiamond = BoolSetting("Diamond", "Destroy diamond ore", false);
    BoolSetting mGold = BoolSetting("Gold", "Destroy gold ore", false);
    BoolSetting mIron = BoolSetting("Iron", "Destroy iron ore", false);
    BoolSetting mCoal = BoolSetting("Coal", "Destroy coal ore", false);
    BoolSetting mRedstone = BoolSetting("Redstone", "Destroy redstone ore", false);
    BoolSetting mLapis = BoolSetting("Lapis", "Destroy lapis ore", false);
    BoolSetting mSwing = BoolSetting("Swing", "Swings when destroying blocks", false);
    BoolSetting mHotbarOnly = BoolSetting("Hotbar Only", "Only switch to tools in the hotbar", false);
    BoolSetting mInfiniteDurability = BoolSetting("Infinite Durability", "Infinite durability for tools (may cause issues!)", false);
    BoolSetting mRenderBlock = BoolSetting("Render Block", "Renders the block you are currently breaking", true);

    OreMiner() : ModuleBase("OreMiner", "Automatically breaks ore", ModuleCategory::Player, 0, false) {
        addSettings(&mUncoverMode, &mRange, &mUncoverRange, &mDestroySpeed, &mDiamond, &mEmerald, &mGold, &mIron, &mCoal, &mRedstone, &mLapis, &mSwing, &mHotbarOnly, &mInfiniteDurability, &mRenderBlock);
        
        mNames = {
            {Lowercase, "oreminer"},
            {LowercaseSpaced, "ore miner"},
            {Normal, "OreMiner"},
            {NormalSpaced, "Ore Miner"}
        };
    }

    struct PathFindResult {
        glm::ivec3 blockPos;
        bool foundPath;
    };

    glm::ivec3 mCurrentBlockPos = { INT_MAX, INT_MAX, INT_MAX };
    glm::ivec3 mTargettingBlockPos = { INT_MAX, INT_MAX, INT_MAX };
    int mCurrentBlockFace = -1;
    float mBreakingProgress = 0.f;
    bool mIsMiningBlock = false;
    bool mWasMiningBlock = false;
    bool mIsUncovering = false;
    bool mShouldRotate = false;
    bool mShouldSpoofSlot = false;
    bool mShouldSetbackSlot = false;
    int mPreviousSlot = -1;
    int mToolSlot = -1;

    uint64_t mLastBlockPlace = 0;
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
    void reset();
    void queueBlock(glm::ivec3 blockPos);
    bool isValidBlock(glm::ivec3 blockPos, bool oreOnly, bool exposedOnly);
    PathFindResult getBestPathToBlock(glm::ivec3 blockPos);

    std::string getSettingDisplay() override {
        return mUncoverMode.mValues[mUncoverMode.as<int>()];
    }
};