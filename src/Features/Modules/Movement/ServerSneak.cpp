//
// Created by vastrakai on 9/2/2024.
//

#include "ServerSneak.hpp"

#include <Features/Events/PacketOutEvent.hpp>
#include <SDK/Minecraft/Network/Packets/PlayerAuthInputPacket.hpp>

void ServerSneak::onEnable()
{
    gFeatureManager->mDispatcher->listen<PacketOutEvent, &ServerSneak::onPacketOutEvent>(this);
}

void ServerSneak::onDisable()
{
    gFeatureManager->mDispatcher->deafen<PacketOutEvent, &ServerSneak::onPacketOutEvent>(this);
}

void ServerSneak::onPacketOutEvent(PacketOutEvent& event)
{
    if (event.mPacket->getId() == PacketID::PlayerAuthInput)
    {
        auto paip = event.getPacket<PlayerAuthInputPacket>();

        paip->mInputData |= AuthInputAction::SNEAK_DOWN | AuthInputAction::SNEAKING | AuthInputAction::START_SNEAKING;
        // remove stop sneaking
        paip->mInputData &= ~AuthInputAction::STOP_SNEAKING;
    }
}
