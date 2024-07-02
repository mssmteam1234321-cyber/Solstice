#pragma once
//
// Created by vastrakai on 7/1/2024.
//

#include <Features/Modules/Module.hpp>


class PacketLogger : public ModuleBase<PacketLogger>
{
public:
    PacketLogger() : ModuleBase("PacketLogger", "Logs packets", ModuleCategory::Misc, 0, false) {}

    void onEnable() override;
    void onDisable() override;
    void onPacketOutEvent(class PacketOutEvent& event);
};