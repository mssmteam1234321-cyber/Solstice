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
    EnumSetting mMode = EnumSetting("Mode", "The mode of the interface.", Trans, {"Trans", "Rainbow", "Custom"});
    ColorSetting mColor1 = ColorSetting("Color 1", "The first color of the interface.", 0xFFFFFFFF);
    ColorSetting mColor2 = ColorSetting("Color 2", "The second color of the interface.", 0x000000FF);
    NumberSetting mColorSpeed = NumberSetting("Color Speed", "The speed of the color change.", 1.f, 1.f, 10.f, 0.01);
    NumberSetting mSaturation = NumberSetting("Saturation", "The saturation of the interface.", 1.f, 0.f, 1.f, 0.01);


    Interface() : ModuleBase("Interface", "Customize the visuals!", ModuleCategory::Visual, 0, true) {
        gFeatureManager->mDispatcher->listen<ModuleStateChangeEvent, &Interface::onModuleStateChange, nes::event_priority::FIRST>(this);
        addSetting(&mMode);
        addSetting(&mColor1);
        addSetting(&mColor2);
        VISIBILITY_CONDITION(mColor1, mMode.mValue == Custom);
        VISIBILITY_CONDITION(mColor2, mMode.mValue == Custom);
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

    void onEnable() override;
    void onDisable() override;
    void onModuleStateChange(ModuleStateChangeEvent& event);

};
