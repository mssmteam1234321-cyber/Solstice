#pragma once
//
// Created by vastrakai on 8/4/2024.
//


class TargetHUD : public ModuleBase<TargetHUD> {
public:
    enum class Style {
        Solstice,
    };

    EnumSettingT<Style> mStyle = EnumSettingT("Style", "The style of the target HUD", Style::Solstice, "Solstice");
    NumberSetting mXOffset = NumberSetting("X Offset", "The X offset of the target HUD", 100, -400, 400, 1);
    NumberSetting mYOffset = NumberSetting("Y Offset", "The Y offset of the target HUD", 100, -400, 400, 1);
    NumberSetting mFontSize = NumberSetting("Font Size", "The size of the font", 20, 1, 40, 1);

    TargetHUD() : ModuleBase("TargetHUD", "Shows target information", ModuleCategory::Visual, 0, false) {
        addSettings(
            &mStyle,
            &mXOffset,
            &mYOffset,
            &mFontSize
        );

        mNames = {
            {Lowercase, "targethud"},
            {LowercaseSpaced, "target hud"},
            {Normal, "TargetHUD"},
            {NormalSpaced, "Target HUD"},
        };

        gFeatureManager->mDispatcher->listen<RenderEvent, &TargetHUD::onRenderEvent>(this);
    }

    float mHealth = 0;
    float mMaxHealth = 0;
    float mLastHealth = 0;
    float mLastMaxHealth = 0;
    float mAbsorption = 0;
    float mMaxAbsorption = 0;
    float mLastAbsorption = 0;
    float mLastMaxAbsorption = 0;
    float mLerpedHealth = 0;
    float mLerpedAbsorption = 0;
    std::string mLastPlayerName = "";
    float mLastHurtTime = 0;
    float mHurtTime = 0;
    Actor* mLastTarget = nullptr;
    constexpr static uint64_t cHurtTimeDuration = 500;

    void onEnable() override;
    void onDisable() override;
    void onBaseTickEvent(class BaseTickEvent& event);
    void onRenderEvent(class RenderEvent& event);
    void onPacketInEvent(class PacketInEvent& event);
};