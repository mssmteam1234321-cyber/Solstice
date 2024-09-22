#pragma once
//
// Created by vastrakai on 9/21/2024.
//


class CustomChat : public ModuleBase<CustomChat> {
public:
    CustomChat() : ModuleBase("CustomChat", "Custom chat!", ModuleCategory::Visual, 0, false) {
        mNames = {
            {Lowercase, "customchat"},
            {LowercaseSpaced, "custom chat"},
            {Normal, "CustomChat"},
            {NormalSpaced, "Custom Chat"},
        };
    }

    std::vector<std::string> mMessages;

    void onEnable() override;
    void onDisable() override;
    void onRenderEvent(class RenderEvent& event);
    void onPacketInEvent(class PacketInEvent& event);
};