#pragma once
#include <Features/Modules/Module.hpp>
//
// Created by alteik on 12/10/2024.
//

class NameProtect : public ModuleBase<NameProtect> {
public:
    NameProtect() : ModuleBase("NameProtect", "changes ur real name (useful for streaming or some)", ModuleCategory::Visual, 0, false) {
        mNames = {
                {Lowercase, "nameprotect"},
                {LowercaseSpaced, "name protect"},
                {Normal, "NameProtect"},
                {NormalSpaced, "Name Protect"}
        };
    }

    std::string mNewName = "discord.gg/stretch";

    std::string mOldLocalName = "";
    std::string mOldNameTag = "";

    void onEnable() override;
    void onDisable() override;
    void onBaseTickEvent(class BaseTickEvent& event);
    void onPacketInEvent(class PacketInEvent& event);
};