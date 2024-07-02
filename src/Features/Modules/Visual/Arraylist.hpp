#pragma once
#include <Features/Modules/Module.hpp>
//
// Created by vastrakai on 7/2/2024.
//


class Arraylist : public ModuleBase<Arraylist>
{
public:
    enum class Style {
        Solstice
    };
    EnumSetting mStyle = EnumSetting("Style", "The style of the watermark.", 0, "Solstice");
    Arraylist() : ModuleBase("Arraylist", "Displays a list of modules", ModuleCategory::Visual, 0, true) {
    }
    void onEnable() override;
    void onDisable() override;

    void onRenderEvent(class RenderEvent& event);
    std::string getSettingDisplay() override {
        return mStyle.mValue == 0 ? "Solstice" : "Unknown";
    }
};