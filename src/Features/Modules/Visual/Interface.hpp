#pragma once
//
// Created by vastrakai on 7/1/2024.
//

#include <Features/FeatureManager.hpp>
#include <Features/Modules/Module.hpp>
#include <Features/Modules/ModuleManager.hpp>
#include <Features/Modules/Setting.hpp>

class Interface : public ModuleBase<Interface> {
public:
    enum ColorTheme {
        Trans,
        Rainbow,
        Custom
    };
    EnumSetting mMode = EnumSetting("Theme  ", "The mode of the interface.", Trans, {"Trans", "Rainbow", "Custom"});
    NumberSetting mColors = NumberSetting("Colors", "The amount of colors in the interface.", 3, 1, 6, 1);
    // Colors will be Fire-themed by default
    ColorSetting mColor1 = ColorSetting("Color 1", "The first color of the interface.", 0xFFFF0000);
    ColorSetting mColor2 = ColorSetting("Color 2", "The second color of the interface.", 0xFFFF7F00);
    ColorSetting mColor3 = ColorSetting("Color 3", "The third color of the interface.", 0xFFFFD600);
    ColorSetting mColor4 = ColorSetting("Color 4", "The fourth color of the interface.", 0xFF00FF00);
    ColorSetting mColor5 = ColorSetting("Color 5", "The fifth color of the interface.", 0xFF0000FF);
    ColorSetting mColor6 = ColorSetting("Color 6", "The sixth color of the interface.", 0xFF8B00FF);
    NumberSetting mColorSpeed = NumberSetting("Color Speed", "The speed of the color change.", 8.f, 0.01f, 10.f, 0.01);
    NumberSetting mSaturation = NumberSetting("Saturation", "The saturation of the interface.", 1.f, 0.f, 1.f, 0.01);


    Interface() : ModuleBase("Interface", "Customize the visuals!", ModuleCategory::Visual, 0, true) {
        gFeatureManager->mDispatcher->listen<ModuleStateChangeEvent, &Interface::onModuleStateChange, nes::event_priority::FIRST>(this);
        addSetting(&mMode);
        addSetting(&mColors);
        addSetting(&mColor1);
        addSetting(&mColor2);
        addSetting(&mColor3);
        addSetting(&mColor4);
        addSetting(&mColor5);
        addSetting(&mColor6);
        VISIBILITY_CONDITION(mColors, mMode.mValue == Custom);
        VISIBILITY_CONDITION(mColor1, mMode.mValue == Custom && mColors.mValue >= 1);
        VISIBILITY_CONDITION(mColor2, mMode.mValue == Custom && mColors.mValue >= 2);
        VISIBILITY_CONDITION(mColor3, mMode.mValue == Custom && mColors.mValue >= 3);
        VISIBILITY_CONDITION(mColor4, mMode.mValue == Custom && mColors.mValue >= 4);
        VISIBILITY_CONDITION(mColor5, mMode.mValue == Custom && mColors.mValue >= 5);
        VISIBILITY_CONDITION(mColor6, mMode.mValue == Custom && mColors.mValue >= 6);
        addSetting(&mColorSpeed);
        addSetting(&mSaturation);
    }

    static inline std::unordered_map<int, std::vector<ImColor>> ColorThemes = {
        {Trans, {
            ImColor(91, 206, 250, 255),
            ImColor(245, 169, 184, 255),
            ImColor(255, 255, 255, 255),
            ImColor(245, 169, 184, 255),
        }},
        {Rainbow, {}},
        {Custom, {}}
    };

    std::vector<ImColor> getCustomColors() {
        auto result = std::vector<ImColor>();
        if (mColors.mValue >= 1) result.push_back(mColor1.getAsImColor());
        if (mColors.mValue >= 2) result.push_back(mColor2.getAsImColor());
        if (mColors.mValue >= 3) result.push_back(mColor3.getAsImColor());
        if (mColors.mValue >= 4) result.push_back(mColor4.getAsImColor());
        if (mColors.mValue >= 5) result.push_back(mColor5.getAsImColor());
        if (mColors.mValue >= 6) result.push_back(mColor6.getAsImColor());
        return result;
    }

    void onEnable() override;
    void onDisable() override;
    void onModuleStateChange(ModuleStateChangeEvent& event);

};
