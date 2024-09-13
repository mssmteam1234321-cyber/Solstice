//
// Created by vastrakai on 9/2/2024.
//

#include "ServerSneak.hpp"

#include <Features/Events/PacketInEvent.hpp>
#include <Features/Events/PacketOutEvent.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/Network/Packets/ActorDataPacket.hpp>
#include <SDK/Minecraft/Network/Packets/PlayerAuthInputPacket.hpp>

void ServerSneak::onEnable()
{
    gFeatureManager->mDispatcher->listen<PacketOutEvent, &ServerSneak::onPacketOutEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketInEvent, &ServerSneak::onPacketInEvent>(this);
}

void ServerSneak::onDisable()
{
    gFeatureManager->mDispatcher->deafen<PacketOutEvent, &ServerSneak::onPacketOutEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketInEvent, &ServerSneak::onPacketInEvent>(this);
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

void ServerSneak::onPacketInEvent(PacketInEvent& event)
{
    auto player = ClientInstance::get()->getLocalPlayer();

    if (event.mPacket->getId() == PacketID::SetActorData && mCancelActorData.mValue)
    {
        auto packet = event.getPacket<ActorDataPacket>();
        if (packet->mId == player->getRuntimeID())
        {
            event.cancel();
        }
    }
}
