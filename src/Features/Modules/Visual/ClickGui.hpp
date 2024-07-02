#pragma once
//
// Created by vastrakai on 6/29/2024.
//

#include <Solstice.hpp>
#include <Features/FeatureManager.hpp>
#include <Features/Modules/Setting.hpp>

#include "spdlog/spdlog.h"

class ClickGui : public ModuleBase<ClickGui> {
public:
    enum ClickGuiStyle {
        Dropdown
    };
    EnumSetting mStyle = EnumSetting("Style", "The style of the ClickGui.", ClickGuiStyle::Dropdown, { "Dropdown" });

    ClickGui() : ModuleBase("ClickGui", "A customizable GUI for toggling modules.", ModuleCategory::Visual, VK_TAB, false) {
        // Register your features here
        gFeatureManager->mDispatcher->listen<RenderEvent, &ClickGui::onRenderEvent>(this);
        spdlog::info("ClickGui module has been constructed.");
    }

    void onEnable() override;
    void onDisable() override;

    void onMouseEvent(class MouseEvent& event);
    void onKeyEvent(class KeyEvent& event);
    void onRenderEvent(class RenderEvent& event);
};

REGISTER_MODULE(ClickGui);