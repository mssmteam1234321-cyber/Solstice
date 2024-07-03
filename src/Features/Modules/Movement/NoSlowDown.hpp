#pragma once
#include <Features/Modules/Module.hpp>
//
// Created by vastrakai on 7/2/2024.
//


class NoSlowDown : public ModuleBase<NoSlowDown> {
public:
    NoSlowDown() : ModuleBase("NoSlowDown", "Prevents you from being slowed down while walking through webs, using items, etc.", ModuleCategory::Movement, 0, false) {
    }

    void onEnable() override;
    void onDisable() override;
    void onBaseTickEvent(class BaseTickEvent& event);
};