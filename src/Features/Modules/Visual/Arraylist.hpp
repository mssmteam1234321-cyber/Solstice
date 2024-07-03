#pragma once
#include <Features/Modules/Module.hpp>
//
// Created by vastrakai on 7/2/2024.
//


class Arraylist : public ModuleBase<Arraylist>
{
public:
    enum class Background {
        None,
        Opacity
    };

    EnumSetting mBackground = EnumSetting("Background", "Background style", 0, "None", "Opacity");
    BoolSetting mGlow = BoolSetting("Glow", "Enables glow", true);
    NumberSetting mGlowStrength = NumberSetting("Glow Strength", "The strength of the glow", 1.f, 0.5f, 1.f, 0.1f);

    Arraylist() : ModuleBase("Arraylist", "Displays a list of modules", ModuleCategory::Visual, 0, true) {
        addSetting(&mBackground);
        addSetting(&mGlow);
        addSetting(&mGlowStrength);
    }
    void onEnable() override;
    void onDisable() override;

    void onRenderEvent(class RenderEvent& event);
    std::string getSettingDisplay() override {
        return mBackground.mValue == 0 ? "None" : "Opacity";
    }
};