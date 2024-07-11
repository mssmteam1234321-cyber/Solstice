#pragma once
//
// Created by vastrakai on 7/10/2024.
//

#include <Features/Modules/Module.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>

class Scaffold : public ModuleBase<Scaffold> {
public:
    enum class RotateMode {
        None,
        Normal,
        Down
    };

    enum class PlacementMode {
        Normal,
        Flareon
    };

    enum class SwitchMode {
        None,
        Full
    };

    enum class SwitchPriority {
        First,
        Highest
    };



    NumberSetting mPlaces = NumberSetting("Places", "The amount of blocks to place per tick", 1, 0, 10, 0.01);
    NumberSetting mRange = NumberSetting("Range", "The range at which to place blocks", 5, 0, 10, 0.01);
    NumberSetting mExtend = NumberSetting("Extend", "The distance to extend the placement", 0, 0, 10, 1);
    EnumSetting mRotateMode = EnumSetting("Rotate Mode", "The mode of rotation", RotateMode::Normal, "None", "Normal", "Down");
    EnumSetting mPlacementMode = EnumSetting("Placement", "The mode for block placement", PlacementMode::Normal, "Normal", "Flareon");
    EnumSetting mSwitchMode = EnumSetting("Switch Mode", "The mode for block switching", SwitchMode::None, "None", "Full");
    EnumSetting mSwitchPriority = EnumSetting("Switch Prio", "The priority for block switching", SwitchPriority::First, "First", "Highest");
    BoolSetting mHotbarOnly = BoolSetting("Hotbar Only", "Whether or not to only place blocks from the hotbar", false);
    BoolSetting mFlareonV2Placement = BoolSetting("Flareon V2", "Whether or not to use Flareon V2 placement", false);
    BoolSetting mLockY = BoolSetting("Lock Y", "Whether or not to lock the Y position", false);
    BoolSetting mSwing = BoolSetting("Swing", "Whether or not to swing the arm", false);

    Scaffold() : ModuleBase("Scaffold", "Automatically places blocks below you", ModuleCategory::Player, 0, false) {
        addSettings(&mPlaces, &mRange, &mExtend, &mRotateMode, &mPlacementMode, &mSwitchMode, &mSwitchPriority, &mHotbarOnly, &mFlareonV2Placement, &mLockY, &mSwing);

        VISIBILITY_CONDITION(mSwitchPriority, mSwitchMode.as<SwitchMode>() != SwitchMode::None);
        VISIBILITY_CONDITION(mHotbarOnly, mSwitchMode.as<SwitchMode>() != SwitchMode::None);

        mNames = {
            {Lowercase, "scaffold"},
            {LowercaseSpaced, "scaffold"},
            {Normal, "Scaffold"},
            {NormalSpaced, "Scaffold"}
        };
    }

    float mStartY = 0;
    glm::vec3 mLastBlock = { 0, 0, 0 };
    int mLastFace = -1;
    bool mShouldRotate = false;
    uint64_t mLastSwitchTime = 0;

    void onEnable() override;
    void onDisable() override;
    void onBaseTickEvent(class BaseTickEvent& event);
    bool tickPlace(class BaseTickEvent& event);
    void onRenderEvent(class RenderEvent& event);
    void onPacketOutEvent(class PacketOutEvent& event);
    glm::vec3 getRotBasedPos(float extend, float yPos);
    glm::vec3 getPlacePos(float extend);

    std::string getSettingDisplay() override {
        return mRotateMode.mValues[mRotateMode.mValue];
    }


};