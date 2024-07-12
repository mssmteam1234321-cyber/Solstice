#pragma once
//
// Created by vastrakai on 6/29/2024.
//

#include <Features/FeatureManager.hpp>
#include <Features/Modules/Setting.hpp>

class Watermark : public ModuleBase<Watermark> {
public:
    enum class Style {
        Solstice,
        SevenDays
    };
    EnumSetting mStyle = EnumSetting("Style", "The style of the watermark.", 0, "Solstice", "7 Days");
    BoolSetting mGlow = BoolSetting("Glow", "Enables glow", true);
    Watermark() : ModuleBase("Watermark", "Displays a watermark on the screen", ModuleCategory::Visual, 0, true) {
        addSetting(&mStyle);
        addSetting(&mGlow);
        gFeatureManager->mDispatcher->listen<RenderEvent, &Watermark::onRenderEvent>(this);

        mNames = {
            {Lowercase, "watermark"},
            {LowercaseSpaced, "watermark"},
            {Normal, "Watermark"},
            {NormalSpaced, "Watermark"}
        };
    }

    void onEnable() override;
    void onDisable() override;

    void onRenderEvent(class RenderEvent& event);

    std::string getSettingDisplay() override {
        return mStyle.mValue == 0 ? "Solstice" : "7 Days";
    }
};
