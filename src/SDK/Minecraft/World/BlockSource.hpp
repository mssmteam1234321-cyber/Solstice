#pragma once
//
// Created by vastrakai on 7/3/2024.
//

#include <Utils/Structs.hpp>

class BlockSource {
public:
    virtual ~BlockSource();
    virtual class Block* getBlock(int, int, int);
    virtual class Block* getBlock(glm::ivec3 const&);
    virtual class Block* getBlock(glm::ivec3 const&, int);
    void clearBlock(const glm::ivec3&);
};