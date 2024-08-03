#pragma once
//
// Created by vastrakai on 7/3/2024.
//

#include <Utils/Structs.hpp>

class BlockSource {
public:
    CLASS_FIELD(int16_t, mBuildHeight, 0x0038);
    CLASS_FIELD(int16_t, mBuildDepth, 0x003A);

    virtual ~BlockSource();
    virtual class Block* getBlock(int, int, int);
    virtual class Block* getBlock(glm::ivec3 const&);
    virtual class Block* getBlock(glm::ivec3 const&, int);
};