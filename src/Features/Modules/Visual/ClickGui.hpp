#pragma once
//
// Created by vastrakai on 6/29/2024.
//

#include <Solstice.hpp>
#include <Features/FeatureManager.hpp>
#include <Features/Modules/Setting.hpp>
#include <Utils/MiscUtils/EasingUtil.hpp>

#include "spdlog/spdlog.h"

class ClickGui : public ModuleBase<ClickGui>
{
public:
    enum ClickGuiStyle {
        Dropdown
    };
    enum ClickGuiAnimation {
        Zoom,
        Bounce
    };
    EnumSetting mStyle = EnumSetting("Style", "The style of the ClickGui.", ClickGuiStyle::Dropdown, { "Dropdown" });
    EnumSetting mAnimation = EnumSetting("Animation", "The animation of the ClickGui.", 0, { "Zoom", "Bounce" });
    //BoolSetting mBlur = BoolSetting("Blur", "Enables blur.", true);
    NumberSetting mBlurStrength = NumberSetting("Blur Strength", "The strength of the blur.", 9.f, 0.f, 20.f, 0.1f);
    NumberSetting mEaseSpeed = NumberSetting("Ease Speed", "The speed of the easing.", 10.5f, 5.f, 15.f, 0.1f);

    ClickGui() : ModuleBase("ClickGui", "A customizable GUI for toggling modules.", ModuleCategory::Visual, VK_TAB, false) {
        // Register your features here
        gFeatureManager->mDispatcher->listen<RenderEvent, &ClickGui::onRenderEvent>(this);
        gFeatureManager->mDispatcher->listen<WindowResizeEvent, &ClickGui::onWindowResizeEvent>(this);
        spdlog::info("ClickGui module has been constructed.");
        addSetting(&mStyle);
        addSetting(&mAnimation);
        //addSetting(&mBlur);
        addSetting(&mBlurStrength);
        addSetting(&mEaseSpeed);
    }

    void onEnable() override;
    void onDisable() override;

    void onWindowResizeEvent(class WindowResizeEvent& event);
    void onMouseEvent(class MouseEvent& event);
    void onKeyEvent(class KeyEvent& event);
    float getEaseAnim(EasingUtil ease, int mode);
    void onRenderEvent(class RenderEvent& event);
};

REGISTER_MODULE(ClickGui);