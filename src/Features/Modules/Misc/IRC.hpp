#pragma once
#include <Features/Events/BaseTickEvent.hpp>
//
// Created by vastrakai on 8/24/2024.
//


class IRC : public ModuleBase<IRC> {
public:
    IRC() : ModuleBase("IRC", "IRC Chat", ModuleCategory::Misc, 0, true) {
        mNames = {
            {Lowercase, "irc"},
            {LowercaseSpaced, "irc"},
            {Normal, "IRC"},
            {NormalSpaced, "IRC"}
        };
    }

    void onEnable() override;
    void onDisable() override;
    void onBaseTickEvent(BaseTickEvent& event);
};