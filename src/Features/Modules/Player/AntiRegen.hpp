#pragma once
#include <Features/Modules/Module.hpp>

class AntiRegen : public ModuleBase<AntiRegen> {
public:
    enum class Mode {
        Cover,
        Slab
    };

    EnumSetting mMode = EnumSetting("Mode", "The antiregen mode", Mode::Cover, "Cover", "Slab");
    NumberSetting mRange = NumberSetting("Range", "The max range for placing blocks", 5, 0, 10, 0.01);
    BoolSetting mSwing = BoolSetting("Swing", "Swings when placing blocks", false);
    BoolSetting mHotbarOnly = BoolSetting("Hotbar Only", "Only switch to blocks in the hotbar", true);

    AntiRegen() : ModuleBase("AntiRegen", "Make ur opponent hard to break redstone", ModuleCategory::Player, 0, false) {
        addSettings(&mMode, &mRange, &mSwing, &mHotbarOnly);

        mNames = {
            {Lowercase, "antiregen"},
            {LowercaseSpaced, "anti regen"},
            {Normal, "AntiRegen"},
            {NormalSpaced, "Anti Regen"}
        };
    }

    std::vector<glm::ivec3> miningRedstones;
    int mPreviousSlot = -1;
    bool mPlacedBlock = false;

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
        return mMode.mValues[mMode.mValue];
    }

};