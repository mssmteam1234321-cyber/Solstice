//
// Created by vastrakai on 7/9/2024.
//

#include "AntiCheatDetector.hpp"

#include <Features/FeatureManager.hpp>
#include <Features/Events/PacketInEvent.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Network/Packets/Packet.hpp>
#include <Utils/GameUtils/ChatUtils.hpp>
#include <Utils/MiscUtils/ColorUtils.hpp>

void AntiCheatDetector::onEnable()
{
    gFeatureManager->mDispatcher->listen<PacketInEvent, &AntiCheatDetector::onPacketInEvent>(this);
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &AntiCheatDetector::onBaseTickEvent>(this);

    mLastCheck = NOW;
    mDetectedAntiCheat = AntiCheats::None;
    ChatUtils::displayClientMessageRaw("§8[§cF§8]§r §7Recalculating...");
    ClientInstance::get()->playUi("random.orb", 1.0f, 0.75f);

}

void AntiCheatDetector::onDisable()
{
    gFeatureManager->mDispatcher->deafen<PacketInEvent, &AntiCheatDetector::onPacketInEvent>(this);
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &AntiCheatDetector::onBaseTickEvent>(this);
}

void AntiCheatDetector::onBaseTickEvent(BaseTickEvent& event)
{
    if (NOW - mLastCheck < 1000) return;
    if (NOW - mLastCheck < 3000 && NOW - mLastPingPacket > 3000) return;
    if (mDetectedAntiCheat != AntiCheats::None) {
        mLastCheck = NOW;
        return;
    }

    bool hasRecentlyPinged = mLastPingPacket > 0 && NOW - mLastPingPacket < 5000 && NOW - mLastCheck > 1000;
    bool hasCheckedAfterPing = mLastCheck > mLastPingPacket;

    if (!hasRecentlyPinged && hasCheckedAfterPing) {
        mDetectedAntiCheat = AntiCheats::FlareonV2;
        ChatUtils::displayClientMessageRaw("§8[§cF§8]§r §cFlareon V2 §fwas detected to be running. The new Anti-Cheat is currently §aactive§f.");
        ClientInstance::get()->playUi("random.orb", 1.0f, 1.0f);
    } else if (hasRecentlyPinged) {
        mDetectedAntiCheat = AntiCheats::FlareonV1;
        if (!V2NotifyOnly.mValue)
        {
            ChatUtils::displayClientMessageRaw("§8[§cF§8]§r §cFlareon V1 §fwas detected to be running. The new Anti-Cheat is currently §cinactive§f.");
            ClientInstance::get()->playUi("random.orb", 1.0f, 0.75f);
        }
    }
}

void AntiCheatDetector::onPacketInEvent(PacketInEvent& event)
{
    if (event.mPacket->getId() == PacketID::ChangeDimension) {
        // Return if the last check was within 2 seconds
        if (NOW - mLastCheck < 2000) return;

        // If the player changes dimension, reset the anti-cheat detection
        mDetectedAntiCheat = AntiCheats::None;
        mLastCheck = NOW;
        mLastPingPacket = 0;
        if (!V2NotifyOnly.mValue)
            ChatUtils::displayClientMessageRaw("§8[§cF§8]§r §7Recalculating...");
    }

    if (event.mPacket->getId() == PacketID::NetworkStackLatency) {
        mLastPingPacket = NOW;
    }
}