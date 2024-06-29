//
// Created by vastrakai on 6/25/2024.
//
#pragma once

#include <Utils/MemUtils.hpp>
#include "EntityContext.hpp"

class Actor {
public:
    CLASS_FIELD(uintptr_t**, vtable, 0x0);
    CLASS_FIELD(EntityContext, mContext, 0x8);
};