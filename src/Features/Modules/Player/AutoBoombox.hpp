#pragma once
//
// 8/11/2024.
//

#include <Features/Modules/Module.hpp>

class AutoBoombox : public ModuleBase<AutoBoombox> {
public:
    NumberSetting mDelay = NumberSetting("Delay", "The delay in ms to place boombox", 1000, 0, 1500, 10);
    BoolSetting mHotbarOnly = BoolSetting{ "Hotbar Only", "Only switch to boomboxes in the hotbar", true };
    AutoBoombox() : ModuleBase("AutoBoombox", "Automatically places boombox in the hive", ModuleCategory::Player, 0, false)
    {
        addSetting(&mDelay);
        addSetting(&mHotbarOnly);

        mNames = {
              {Lowercase, "autoboombox"},
                {LowercaseSpaced, "auto boombox"},
                {Normal, "AutoBoombox"},
                {NormalSpaced, "Auto Boombox"}
        };
    };

    std::vector<glm::vec3> mPlaceOffsets = {
        glm::ivec3(2, 0, 0),
        glm::ivec3(-2, 0, 0),
        glm::ivec3(0, 0, 2),
        glm::ivec3(0, 0, -2),
    };

    int mPreviousSlot = 0;
    bool mSelectedSlot = false;
    bool mShouldRotate = false;

    uint64_t mLastBoomboxPlace = 0;
    glm::ivec3 mCurrentPlacePos = { INT_MAX, INT_MAX, INT_MAX };

    void onEnable();
    void onDisable() override;
    void onBaseTickEvent(class BaseTickEvent& event);
    void onPacketOutEvent(class PacketOutEvent& event);
};