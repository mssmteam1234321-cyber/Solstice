//
// Created by vastrakai on 7/19/2024.
//

#include "NoPacket.hpp"

#include <Features/Events/PacketOutEvent.hpp>

void NoPacket::onEnable()
{
    gFeatureManager->mDispatcher->listen<PacketOutEvent, &NoPacket::onPacketOutEvent>(this);
}

void NoPacket::onDisable()
{
    gFeatureManager->mDispatcher->deafen<PacketOutEvent, &NoPacket::onPacketOutEvent>(this);
}

void NoPacket::onPacketOutEvent(PacketOutEvent& event) const
{
    if ((mMoveOnly && (event.packet->getId() == PacketID::MovePlayer || event.packet->getId() == PacketID::PlayerAuthInput)) || !mMoveOnly)
        event.mCancelled = true;
}
