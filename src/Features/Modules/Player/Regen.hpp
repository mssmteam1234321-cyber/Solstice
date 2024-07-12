#pragma once
#include <Features/Modules/Module.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/World/Block.hpp>
#include <SDK/Minecraft/World/BlockSource.hpp>
#include <SDK/Minecraft/World/BlockLegacy.hpp>

class Regen : public ModuleBase<Regen> {
public:
    NumberSetting mRange = NumberSetting("Range", "The range for destroying blocks", 5, 0, 8, 0.01);
    NumberSetting mDestroySpeed = NumberSetting("Destroy Speed", "The destroy speed for regen", 1, 0, 1, 0.01);
    BoolSetting mSwing = BoolSetting("Swing", "Should swing arm when broke block", false);
    BoolSetting mUncover = BoolSetting("Uncover", "Uncover redstone if there are no exposed redstones around you", false);

    Regen() : ModuleBase("Regen", "Get extra health in hive automatically", ModuleCategory::Player, 0, false) {
        addSettings(&mRange, &mDestroySpeed, &mSwing, &mUncover);

        mNames = {
            {Lowercase, "regen"},
            {LowercaseSpaced, "regen"},
            {Normal, "Regen"},
            {NormalSpaced, "Regen"}
        };
    }

    glm::ivec3 mCurrentBlockPos = { 0, 0, 0 };
    glm::ivec3 mTargettingBlockPos = { 0, 0, 0 };
    int mCurrentBlockFace = -1;
    float mBreakingProgress = 0.f;
    bool mIsMiningBlock = false;
    bool mIsUncovering = false;
    bool mShouldRotate = false;
    int mPreviousSlot = -1;
    int mToolSlot = -1;

    void onEnable() override;
    void onDisable() override;
    void onBaseTickEvent(class BaseTickEvent& event);
    void onRenderEvent(class RenderEvent& event);
    void onPacketOutEvent(class PacketOutEvent& event);
    void initializeRegen();
    void queueBlock(glm::ivec3 blockPos);
    bool isValidBlock(glm::ivec3 blockPos, bool redstoneOnly, bool exposedOnly);

    std::string getSettingDisplay() override {
        return std::to_string(static_cast<int>(mRange.mValue));
    }

};