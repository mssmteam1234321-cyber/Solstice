//
// Created by vastrakai on 6/28/2024.
//

#pragma once

#include <Hook/Hook.hpp>
#include <Hook/HookManager.hpp>
#include <SDK/OffsetProvider.hpp>
#include <SDK/SigManager.hpp>
#include <SDK/Minecraft/Network/Packets/Packet.hpp>

class PacketSendHook : public Hook {
public:
    PacketSendHook() : Hook() {

    }

    static std::unique_ptr<Detour> mDetour;

    static void* onPacketSend(void* _this, class Packet* packet);
    void init() override;
};

REGISTER_HOOK(PacketSendHook);