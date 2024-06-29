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
    virtual void send(void* packet) = 0;
    virtual void sendToServer(void* packet) = 0;
    // (none of the other funcs here matter)
};