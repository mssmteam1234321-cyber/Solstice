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

std::unique_ptr<Detour> PacketSendHook::mDetour = nullptr;

void* PacketSendHook::onPacketSend(void* _this, Packet *packet) {
    // Call the original function
    auto original = mDetour->getOriginal<decltype(&onPacketSend)>();
    if (packet->getId() == PacketID::Text) {
        auto textPacket = reinterpret_cast<TextPacket*>(packet);
        Solstice::console->info("PacketSendHook::onPacketSend: {}", textPacket->mMessage);
    }

    if (packet->getId() == PacketID::PlayerAuthInput) {
        auto paip = reinterpret_cast<PlayerAuthInputPacket*>(packet);
        paip->mPos += glm::vec3{ 0.f, 0.1f, 0.f };
        paip->mMove = glm::vec2{ 0.f, 1.f };
        paip->mInputData |= AuthInputAction::START_GLIDING;
        paip->mInputData &= ~AuthInputAction::STOP_GLIDING;
    }
    return original(_this, packet);
}



void PacketSendHook::init() {
    mName = "LoopbackPacketSender::send";
    mDetour = std::make_unique<Detour>("LoopbackPacketSender::send", reinterpret_cast<void*>(ClientInstance::get()->getPacketSender()->vtable[1]), &PacketSendHook::onPacketSend);
    mDetour->enable();
}
