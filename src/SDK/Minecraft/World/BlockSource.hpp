#pragma once
//
// Created by vastrakai on 7/3/2024.
//

#include <Utils/Structs.hpp>

#include "Chunk/ChunkSource.hpp"

class BlockSource {
public:
    CLASS_FIELD(int16_t, mBuildHeight, 0x0038);
    CLASS_FIELD(int16_t, mBuildDepth, 0x003A);

    virtual ~BlockSource();
    virtual Block* getBlock(int, int, int);
    virtual Block* getBlock(glm::ivec3 const&);
    virtual Block* getBlock(glm::ivec3 const&, int);
    LevelChunk* getChunk(ChunkPos const&);
    void setBlock(BlockPos const&, Block*);
};
