//
// Created by vastrakai on 7/1/2024.
//

#include "PacketLogger.hpp"

#include <Features/FeatureManager.hpp>

#include "spdlog/spdlog.h"
#include <SDK/Minecraft/Network/Packets/Packet.hpp>
#include <Features/Events/PacketOutEvent.hpp>

void PacketLogger::onEnable()
{
    gFeatureManager->mDispatcher->listen<PacketOutEvent, &PacketLogger::onPacketOutEvent>(this);
}

void PacketLogger::onDisable()
{
    gFeatureManager->mDispatcher->deafen<PacketOutEvent, &PacketLogger::onPacketOutEvent>(this);
}

void PacketLogger::onPacketOutEvent(PacketOutEvent& event)
{
    // Log the packet
    spdlog::info("Packet: {}", event.packet->getName());
}
