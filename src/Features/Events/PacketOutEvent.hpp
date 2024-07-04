//
// Created by vastrakai on 6/30/2024.
//

#pragma once

#include "Event.hpp"
#include <SDK/Minecraft/Network/Packets/Packet.hpp>

class PacketOutEvent : public CancellableEvent {
public:
    Packet* packet;

    explicit PacketOutEvent(Packet* packet) : CancellableEvent() {
        this->packet = packet;
    }

    template <typename T>
    T* getPacket() const
    {
        return reinterpret_cast<T*>(packet);
    }
};