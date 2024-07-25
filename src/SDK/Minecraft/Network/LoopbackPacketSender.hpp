#pragma once
//
// Created by vastrakai on 6/28/2024.
//

#include <Utils/MemUtils.hpp>

class LoopbackPacketSender {
public:
    CLASS_FIELD(uintptr_t**, vtable, 0x0);
    CLASS_FIELD(class NetworkSystem*, mNetworkSystem, 0x20);

    virtual ~LoopbackPacketSender() = 0;
    /// <summary>
    /// Sends a packet to the server.
    /// Packets sent with this function will trigger the PacketOutEvent.
    /// </summary>
    /// <param name="packet">The packet to send.</param>
    virtual void send(void* packet) = 0;
    /// <summary>
    /// Sends a packet to the server.
    /// Calling this function will NOT trigger the PacketOutEvent.
    /// </summary>
    /// <param name="packet">The packet to send.</param>
    virtual void sendToServer(void* packet) = 0;

    // (none of the other funcs here matter)
};