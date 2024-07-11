#pragma once
//
// Created by vastrakai on 7/10/2024.
//
#include <Utils/MemUtils.hpp>

class Level {
public:
    CLASS_FIELD(uintptr_t**, mVfTable, 0x0);

    class HitResult* getHitResult();

};