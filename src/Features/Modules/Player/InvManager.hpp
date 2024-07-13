#pragma once
//
// Created by vastrakai on 7/6/2024.
//

#include <Features/Modules/Module.hpp>
#include <Utils/MiscUtils/ColorUtils.hpp>

class InvManager : public ModuleBase<InvManager> {
public:
    enum class Mode {
        Instant,
        Delayed
    };

    std::vector<std::string> mSlots = {
        "None",
        "1",
        "2",
        "3",
        "4",
        "5",
        "6",
        "7",
        "8",
        "9"
    };

    EnumSetting mMode = EnumSetting("Mode", "The mode of the module", 0, "Instant", "Delayed");
    NumberSetting mDelay = NumberSetting("Delay", "The delay, in milliseconds", 50, 0, 500, 1);
    BoolSetting mPreferredSlots = BoolSetting("Preferred Slots", "Use preferred slots", true);
    EnumSetting mPreferredSwordSlot = EnumSetting("Sword Slot", "The slot where your sword is", 1, mSlots);
    EnumSetting mPreferredPickaxeSlot = EnumSetting("Pickaxe Slot", "The slot where your pickaxe is", 2, mSlots);
    EnumSetting mPreferredAxeSlot = EnumSetting("Axe Slot", "The slot where your axe is", 3, mSlots);
    EnumSetting mPreferredShovelSlot = EnumSetting("Shovel Slot", "The slot where your shovel is", 4, mSlots);
    BoolSetting mSpoofOpen = BoolSetting("Spoof Open", "Server-sidedly opens your inventory while performing item actions, can bypass some anticheats", false);


    InvManager() : ModuleBase("InvManager", "Manages your inventory", ModuleCategory::Player, 0, false) {
        addSetting(&mMode);
        addSetting(&mDelay);
        addSetting(&mPreferredSlots);
        addSetting(&mPreferredSwordSlot);
        addSetting(&mPreferredPickaxeSlot);
        addSetting(&mPreferredAxeSlot);
        addSetting(&mPreferredShovelSlot);
        addSetting(&mSpoofOpen);

        VISIBILITY_CONDITION(mPreferredSwordSlot, mPreferredSlots.mValue);
        VISIBILITY_CONDITION(mPreferredPickaxeSlot, mPreferredSlots.mValue);
        VISIBILITY_CONDITION(mPreferredAxeSlot, mPreferredSlots.mValue);
        VISIBILITY_CONDITION(mPreferredShovelSlot, mPreferredSlots.mValue);
        VISIBILITY_CONDITION(mDelay, mMode.mValue == static_cast<int>(Mode::Delayed));

        mNames = {
            {Lowercase, "invmanager"},
            {LowercaseSpaced, "inv manager"},
            {Normal, "InvManager"},
            {NormalSpaced, "Inv Manager"}
        };
    }

    int64_t mLastAction = NOW;
    int64_t mLastPing = 0;
    bool mCloseNext = false;

    void onEnable() override;
    void onDisable() override;
    void onBaseTickEvent(class BaseTickEvent& event);
    void onPacketInEvent(class PacketInEvent& event);
    void onPacketOutEvent(class PacketOutEvent& event);
    void onPingUpdateEvent(class PingUpdateEvent& event);
    static bool isItemUseless(class ItemStack* item, int slot);

    std::string getSettingDisplay() override {
        return mMode.mValue == static_cast<int>(Mode::Instant) ? "Instant" : std::to_string(static_cast<int>(mDelay.mValue));
    }
};
