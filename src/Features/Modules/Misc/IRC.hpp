#pragma once
#include <Features/Events/BaseTickEvent.hpp>
//
// Created by vastrakai on 8/24/2024.
//


class IRC : public ModuleBase<IRC> {
public:
    BoolSetting mShowNamesInChat = BoolSetting("Show Names in Chat", "Shows the names of IRC users in the normal minecraft chat when they send a message", true);
    IRC() : ModuleBase("IRC", "IRC Chat", ModuleCategory::Misc, 0, false) {

        addSetting(&mShowNamesInChat);

        mNames = {
            {Lowercase, "irc"},
            {LowercaseSpaced, "irc"},
            {Normal, "IRC"},
            {NormalSpaced, "IRC"}
        };

        gFeatureManager->mDispatcher->listen<BaseTickEvent, &IRC::onBaseTickEvent>(this);
    }

    void onEnable() override;
    void onDisable() override;
    void onBaseTickEvent(BaseTickEvent& event);
};