#pragma once
//
// Created by vastrakai on 7/3/2024.
//

#include <optional>
#include <vector>
#include <Utils/MemUtils.hpp>

#include <glm/glm.hpp>
#include <Utils/Structs.hpp>

class BlockLegacy {
public:
    CLASS_FIELD(uintptr_t**, mVfTable, 0x0);
    CLASS_FIELD(std::string, mTileName, 0x28);
    CLASS_FIELD(std::string, mName, 0xA0);
    CLASS_FIELD(bool, mSolid, 0x164);

    uint16_t getBlockId();
    bool mayPlaceOn(glm::ivec3 pos);
    bool isAir();
};