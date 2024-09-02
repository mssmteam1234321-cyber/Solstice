#pragma once
//
// Created by vastrakai on 9/2/2024.
//


class ServerSneak : public ModuleBase<ServerSneak>
{
public:
    ServerSneak() : ModuleBase("ServerSneak", "Sneak server-sidedly", ModuleCategory::Movement, 0, false)
    {
        mNames = {
            {Lowercase, "serversneak"},
            {LowercaseSpaced, "server sneak"},
            {Normal, "ServerSneak"},
            {NormalSpaced, "Server Sneak"}
        };
    }

    void onEnable() override;
    void onDisable() override;
    void onPacketOutEvent(class PacketOutEvent& event);
};