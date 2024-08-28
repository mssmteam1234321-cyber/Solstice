#pragma once
#include <Features/Events/BaseTickEvent.hpp>
//
// Created by vastrakai on 8/24/2024.
//


class IRC : public ModuleBase<IRC> {
public:
    BoolSetting mShowNamesInChat = BoolSetting("Show Names in Chat", "Shows the names of IRC users in the normal minecraft chat when they send a message", true);
    BoolSetting mAlwaysSendToIrc = BoolSetting("Always Send to IRC", "Sends all chat messages to IRC without prefixing them with #", false);
    IRC() : ModuleBase("IRC", "Chat with other players using IRC", ModuleCategory::Misc, 0, false) {
        addSetting(&mShowNamesInChat);
        addSetting(&mAlwaysSendToIrc);

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