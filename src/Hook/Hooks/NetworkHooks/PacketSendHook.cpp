//
// Created by vastrakai on 6/28/2024.
//
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Network/LoopbackPacketSender.hpp>
#include <Solstice.hpp>
#include <SDK/Minecraft/Network/Packets/TextPacket.hpp>
#include <SDK/Minecraft/Network/Packets/PlayerAuthInputPacket.hpp>
#include <glm/gtc/random.hpp>
#include "PacketSendHook.hpp"

#include <Features/FeatureManager.hpp>
#include <Features/Command/CommandManager.hpp>

std::unique_ptr<Detour> PacketSendHook::mDetour = nullptr;

void* PacketSendHook::onPacketSend(void* _this, Packet *packet) {
    // Call the original function
    auto original = mDetour->getOriginal<decltype(&onPacketSend)>();

    if (packet->getId() == PacketID::Text)
    {
        auto textPacket = reinterpret_cast<TextPacket*>(packet);
        if (gFeatureManager && gFeatureManager->mCommandManager)
        {
            bool cancel = false;
            gFeatureManager->mCommandManager->handleCommand(textPacket->mMessage, &cancel);
            if (cancel)
            {
                return nullptr;
            }
        }
    }

    return original(_this, packet);
}



void PacketSendHook::init() {
    mName = "LoopbackPacketSender::send";
    mDetour = std::make_unique<Detour>("LoopbackPacketSender::send", reinterpret_cast<void*>(ClientInstance::get()->getPacketSender()->vtable[1]), &PacketSendHook::onPacketSend);
    mDetour->enable();
}
