#pragma once
//
// Created by vastrakai on 9/8/2024.
//


class AntiVoid : public ModuleBase<AntiVoid> {
public:
    NumberSetting mFallDistance = NumberSetting("Fall Distance", "The distance you can fall before being teleported back", 5, 1, 15, 1);
    BoolSetting mTpOnce = BoolSetting("Teleport Once", "Only teleport once", false);

    AntiVoid() : ModuleBase("AntiVoid", "Prevents you from falling into the void", ModuleCategory::Player, 0, false) {
        addSetting(&mFallDistance);
        addSetting(&mTpOnce);

        mNames = {
            {Lowercase, "antivoid"},
            {LowercaseSpaced, "anti void"},
            {Normal, "AntiVoid"},
            {NormalSpaced, "Anti Void"}
        };
    }

    glm::vec3 mLastOnGroundPos = { FLT_MAX, FLT_MAX, FLT_MAX };
    bool mTeleported = false;

    void onEnable() override;
    void onDisable() override;
    void onBaseTickEvent(class BaseTickEvent& event);
};