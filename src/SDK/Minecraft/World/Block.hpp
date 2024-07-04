#pragma once
//
// Created by vastrakai on 7/3/2024.
//

#include <Utils/MemUtils.hpp>

class Block {
public:
    CLASS_FIELD(class BlockLegacy*, mLegacy, 0x30);

    BlockLegacy* toLegacy() {
        return mLegacy;
    }
};