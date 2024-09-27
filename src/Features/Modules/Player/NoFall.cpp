//
// Created by vastrakai on 8/30/2024.
//

#include "NoFall.hpp"

#include <Features/Events/PacketOutEvent.hpp>
#include <SDK/Minecraft/Network/Packets/PlayerAuthInputPacket.hpp>

void NoFall::onEnable()
{
    gFeatureManager->mDispatcher->listen<PacketOutEvent, &NoFall::onPacketOutEvent>(this);
}

void NoFall::onDisable()
{
    gFeatureManager->mDispatcher->deafen<PacketOutEvent, &NoFall::onPacketOutEvent>(this);
}

void NoFall::onPacketOutEvent(PacketOutEvent& event)
{
    if (event.mPacket->getId() == PacketID::PlayerAuthInput)
    {
        auto paip = event.getPacket<PlayerAuthInputPacket>();
        if (mMode.mValue == Mode::Sentinel) {
            static bool alt = false;
            alt = !alt;
            if (alt) return;
            paip->mPosDelta.y = -0.0784000015258789f;
        }
        else if (mMode.mValue == Mode::BDS) {
            paip->mInputData |= AuthInputAction::START_GLIDING;
            paip->mInputData &= ~AuthInputAction::STOP_GLIDING;
            paip->mPosDelta = { 0, 0, 0 };
        }
    }
}
