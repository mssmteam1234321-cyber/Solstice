#pragma once
#include <complex.h>
#include <SDK/Minecraft/Network/Packets/Packet.hpp>
//
// Created by vastrakai on 7/5/2024.
//


class PacketUtils {
public:
    static void spoofSlot(int slot);
    static void sendToSelf(std::shared_ptr<Packet> packet);
    static void sendChatMessage(const std::string& msg);
};