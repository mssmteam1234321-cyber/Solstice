//
// Created by vastrakai on 8/13/2024.
//

#pragma once

#include "Packet.hpp"
#include <SDK/Minecraft/JSON.hpp>

enum class ModalFormCancelReason : signed char {
    UserClosed = 0x0,
    UserBusy   = 0x1,
};

class ModalFormResponsePacket : public Packet {
public:
    static inline PacketID ID = PacketID::ModalFormResponse;

    unsigned int                         mFormId;           // this+0x30
    std::optional<MinecraftJson::Value>  mJSONResponse;     // this+0x38
    std::optional<ModalFormCancelReason> mFormCancelReason; // this+0x50
};

static_assert(sizeof(ModalFormResponsePacket) == 0x48);