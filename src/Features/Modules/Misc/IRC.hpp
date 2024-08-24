#pragma once
//
// Created by vastrakai on 8/24/2024.
//


class IRC : public ModuleBase<IRC> {
public:
    IRC() : ModuleBase("IRC", "IRC Chat", ModuleCategory::Misc, 0, false) {
        mNames = {
            {Lowercase, "irc"},
            {LowercaseSpaced, "irc"},
            {Normal, "IRC"},
            {NormalSpaced, "IRC"}
        };
    }

    void onEnable() override;
    void onDisable() override;
};