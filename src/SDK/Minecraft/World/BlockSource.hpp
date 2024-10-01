#pragma once
//
// Created by vastrakai on 7/3/2024.
//

#include <Utils/Structs.hpp>

#include "Chunk/ChunkSource.hpp"

class BlockSource {
public:
    CLASS_FIELD(uintptr_t**, mVfTable, 0x0);
    int16_t getBuildHeight() {
        return hat::member_at<int16_t>(this, OffsetProvider::BlockSource_mBuildHeight);
    }

    int16_t getBuildDepth() {
        return hat::member_at<int16_t>(this, OffsetProvider::BlockSource_mBuildHeight + 2);
    }

    virtual ~BlockSource();
    virtual Block* getBlock(int, int, int);
    virtual Block* getBlock(glm::ivec3 const&);
    virtual Block* getBlock(glm::ivec3 const&, int);
    LevelChunk* getChunk(ChunkPos const&);
    void setBlock(BlockPos const&, Block*);

    class HitResult checkRayTrace(glm::vec3 start, glm::vec3 end, void *player = nullptr);
};
