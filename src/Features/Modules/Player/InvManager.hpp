#pragma once
//
// Created by vastrakai on 7/6/2024.
//

#include <Features/Modules/Module.hpp>

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

    enum class SlotPreference {
        None,
        Slot1,
        Slot2,
        Slot3,
        Slot4,
        Slot5,
        Slot6,
        Slot7,
        Slot8,
        Slot9
    };

    EnumSettingT<Mode> mMode = EnumSettingT<Mode>("Mode", "The mode of the module", Mode::Instant, "Instant", "Delayed");
    NumberSetting mDelay = NumberSetting("Delay", "The delay, in milliseconds", 50, 0, 500, 1);
    BoolSetting mPreferredSlots = BoolSetting("Preferred Slots", "Use preferred slots", true);
    EnumSettingT<int> mPreferredSwordSlot = EnumSettingT("Sword Slot", "The slot where your sword is", 1, mSlots);
    EnumSettingT<int> mPreferredPickaxeSlot = EnumSettingT("Pickaxe Slot", "The slot where your pickaxe is", 2, mSlots);
    EnumSettingT<int> mPreferredAxeSlot = EnumSettingT("Axe Slot", "The slot where your axe is", 3, mSlots);
    EnumSettingT<int> mPreferredShovelSlot = EnumSettingT("Shovel Slot", "The slot where your shovel is", 4, mSlots);
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
        VISIBILITY_CONDITION(mDelay, mMode.mValue == Mode::Delayed);

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
        return mMode.mValue == Mode::Instant ? "Instant" : std::to_string(static_cast<int>(mDelay.mValue));
    }
};
