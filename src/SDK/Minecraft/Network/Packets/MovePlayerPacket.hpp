//
// Created by vastrakai on 7/3/2024.
//

#pragma once

#include "Packet.hpp"
#include <SDK/Minecraft/Actor/ActorType.hpp>
#include <glm/glm.hpp>

enum class PositionMode : unsigned char {
    Normal      = 0,
    Respawn     = 1,
    Teleport    = 2,
    OnlyHeadRot = 3,
};

enum class TeleportationCause : int {
    Unknown     = 0x0,
    Projectile  = 0x1,
    ChorusFruit = 0x2,
    Command     = 0x3,
    Behavior    = 0x4,
    Count       = 0x5,
};

class MovePlayerPacket : public Packet {
public:
    int64_t                                   mPlayerID;         // this+0x30
    glm::vec3                                 mPos;              // this+0x38
    glm::vec2                                 mRot;              // this+0x44
    float                                     mYHeadRot;         // this+0x4C
    PositionMode                              mResetPosition;    // this+0x50
    bool                                      mOnGround;         // this+0x51
    int64_t                                   mRidingID;         // this+0x58
    TeleportationCause                        mCause;            // this+0x60
    ActorType                                 mSourceEntityType; // this+0x64
    uint64_t                                  mTick;             // this+0x68
};