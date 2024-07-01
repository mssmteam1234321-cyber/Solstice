//
// Created by vastrakai on 6/30/2024.
//

#pragma once

#include <glm/vec3.hpp>

#include "glm/vec2.hpp"

struct MoveInputComponent {
public:
    CLASS_FIELD(bool, mIsMoveLocked, 0x82);
    CLASS_FIELD(bool, mIsSneakDown, 0x20);
    CLASS_FIELD(bool, mIsJumping, 0x26);
    CLASS_FIELD(bool, mIsJumping2, 0x80);
    CLASS_FIELD(bool, mIsSprinting, 0x27);
    CLASS_FIELD(bool, mForward, 0x2A);
    CLASS_FIELD(bool, mBackward, 0x2B);
    CLASS_FIELD(bool, mLeft, 0x2C);
    CLASS_FIELD(bool, mRight, 0x2D);
    CLASS_FIELD(glm::vec2, mMoveVector, 0x48);

    // padding to make the struct size 136
    char pad_0x0[0x88];

    void setJumping(bool value) {
        reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(this) + 0x26)[0] = value;
        reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(this) + 0x80)[0] = value;
    }
};

static_assert(sizeof(MoveInputComponent) == 136, "MoveInputComponent size is not 136 bytes!");
