//
// Created by vastrakai on 7/1/2024.
//

#include "PacketLogger.hpp"

#include <Features/FeatureManager.hpp>
#include <Features/Events/PacketInEvent.hpp>

#include "spdlog/spdlog.h"
#include <SDK/Minecraft/Network/Packets/Packet.hpp>
#include <Features/Events/PacketOutEvent.hpp>

void PacketLogger::onEnable()
{
    gFeatureManager->mDispatcher->listen<PacketOutEvent, &PacketLogger::onPacketOutEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketInEvent, &PacketLogger::onPacketInEvent>(this);
}

void PacketLogger::onDisable()
{
    gFeatureManager->mDispatcher->deafen<PacketOutEvent, &PacketLogger::onPacketOutEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketInEvent, &PacketLogger::onPacketInEvent>(this);
}

void PacketLogger::onPacketOutEvent(PacketOutEvent& event)
{
    if (!mOutgoing.mValue) return;
    // Log the packet
    spdlog::info("[out] Packet: {}", event.packet->getName());
    if (event.packet->getId() == PacketID::Login)
    {
        auto packet = event.getPacket<Packet>();
        spdlog::info("Packet: {}", "Login");
    };
}

std::vector<PacketID> ignored = { PacketID::LevelChunk, PacketID::MoveActorAbsolute, PacketID::MoveActorDelta, PacketID::UpdateAttributes, PacketID::SetActorData };

void PacketLogger::onPacketInEvent(PacketInEvent& event)
{
    if (!mIncoming.mValue) return;
    if (std::find(ignored.begin(), ignored.end(), event.mPacket->getId()) != ignored.end()) return;
    spdlog::info("[in] Packet: {}", event.mPacket->getName());
}
