#pragma once
#include <Features/Modules/Module.hpp>

class AntiRegen : public ModuleBase<AntiRegen> {
public:
    enum class Mode {
        Cover
    };

    EnumSettingT<Mode> mMode = EnumSettingT<Mode>("Mode", "The antiregen mode", Mode::Cover, "Cover");
    NumberSetting mRange = NumberSetting("Range", "The max range for placing blocks", 5, 0, 10, 0.01);
    BoolSetting mSwing = BoolSetting("Swing", "Swings when placing blocks", false);
    BoolSetting mHotbarOnly = BoolSetting("Hotbar Only", "Only switch to blocks in the hotbar", true);
    BoolSetting mCancelHit = BoolSetting("Cancel Attack", "Cancel attack packet when placing blocks", false);
    BoolSetting mInputSpoof = BoolSetting("Input Spoof", "Spoof input mode when placing blocks (it doesn't do anything lol)", false);

    AntiRegen() : ModuleBase("AntiRegen", "Automatically places blocks to prevent regeneration", ModuleCategory::Player, 0, false) {
        addSettings(&mMode, &mRange, &mSwing, &mHotbarOnly, &mCancelHit, &mInputSpoof);

        mNames = {
            {Lowercase, "antiregen"},
            {LowercaseSpaced, "anti regen"},
            {Normal, "AntiRegen"},
            {NormalSpaced, "Anti Regen"}
        };
    }

    std::vector<glm::ivec3> miningRedstones;
    glm::ivec3 mCurrentPlacePos = { 0, 0, 0 };
    int mPreviousSlot = -1;
    bool mPlacedBlock = false;
    bool mShouldRotate = false;
    bool mShouldCancelAttack = false;

    std::vector<glm::ivec3> offsetList = {
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
    void onPacketOutEvent(class PacketOutEvent& event);
    void onPacketInEvent(class PacketInEvent& event);
    bool isValidRedstone(glm::ivec3 blockPos);

    std::string getSettingDisplay() override {
        return mMode.mValues[mMode.as<int>()];
    }

};