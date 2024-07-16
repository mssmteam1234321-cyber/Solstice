#pragma once
#include <Features/Modules/Module.hpp>
//
// Created by vastrakai on 7/2/2024.
//


class Arraylist : public ModuleBase<Arraylist>
{
public:

    enum class Display {
        Outline,
        Bar,
        None
    };

    enum class ModuleVisibility {
        All,
        Bound,
    };

    //EnumSetting mBackground = EnumSetting("Background", "Background style", 0, "None", "Opacity");
    NumberSetting mBackgroundOpacity = NumberSetting("Opacity", "The opacity of the background", 0.f, 0.0f, 1.f, 0.01f);
    EnumSetting mDisplay = EnumSetting("Display", "Outline style", Display::Bar, "Outline", "Bar", "None");
    EnumSetting mVisibility = EnumSetting("Visibility", "Module visibility", ModuleVisibility::All, "All", "Bound");
    BoolSetting mRenderMode = BoolSetting("Render Mode", "Renders the module mode next to the module name", true);
    BoolSetting mGlow = BoolSetting("Glow", "Enables glow", true);
    NumberSetting mGlowStrength = NumberSetting("Glow Strength", "The strength of the glow", 1.f, 0.5f, 1.f, 0.1f);

    Arraylist() : ModuleBase("Arraylist", "Displays a list of modules", ModuleCategory::Visual, 0, true) {
        addSetting(&mBackgroundOpacity);
        addSetting(&mDisplay);
        addSetting(&mVisibility);
        addSetting(&mRenderMode);
        addSetting(&mGlow);
        addSetting(&mGlowStrength);

        mNames = {
            {Lowercase, "arraylist"},
            {LowercaseSpaced, "array list"},
            {Normal, "Arraylist"},
            {NormalSpaced, "Array List"}
        };
    }
    void onEnable() override;
    void onDisable() override;

    void onRenderEvent(class RenderEvent& event);
    std::string getSettingDisplay() override {
          return mDisplay.mValues[mDisplay.mValue];
    }
};