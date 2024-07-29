//
// Created by vastrakai on 7/7/2024.
//

#include "BlockUtils.hpp"

#include <Features/Events/BlockChangedEvent.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/Actor/GameMode.hpp>
#include <SDK/Minecraft/Inventory/PlayerInventory.hpp>
#include <SDK/Minecraft/Network/MinecraftPackets.hpp>
#include <SDK/Minecraft/World/BlockSource.hpp>
#include <SDK/Minecraft/World/BlockLegacy.hpp>
#include <SDK/Minecraft/World/HitResult.hpp>
#include <SDK/Minecraft/World/Level.hpp>
#include <SDK/Minecraft/Network/Packets/PlayerActionPacket.hpp>
#include <SDK/Minecraft/Network/Packets/InventoryTransactionPacket.hpp>
#include <SDK/Minecraft/Network/Packets/UpdateBlockPacket.hpp>

std::vector<BlockInfo> BlockUtils::getBlockList(const glm::ivec3& position, float r)
{
    std::vector<BlockInfo> blocks = {};

    const auto blockSource = ClientInstance::get()->getBlockSource();

    auto newBlocks = std::vector<BlockInfo>();


    const int radius = static_cast<int>(r);
    newBlocks.reserve(radius * radius * radius); // reserve enough space for all blocks

    for (int x = position.x - radius; x < position.x + radius; x++)
        for (int y = position.y - radius; y < position.y + radius; y++)
            for (int z = position.z - radius; z < position.z + radius; z++)
                if (const auto block = blockSource->getBlock({ x, y, z }))
                    newBlocks.push_back({ block, { x, y, z } });


    return newBlocks;
}

int BlockUtils::getBlockPlaceFace(glm::ivec3 blockPos)
{
    for (auto& [face, offset] : blockFaceOffsets)
        if (!isAirBlock(blockPos + glm::ivec3(offset))) return face;
    return -1;
}

int BlockUtils::getExposedFace(glm::ivec3 blockPos)
{
    static std::vector<glm::ivec3> offsetList = {
        glm::ivec3(0, -1, 0),
        glm::ivec3(0, 1, 0),
        glm::ivec3(0, 0, -1),
        glm::ivec3(0, 0, 1),
        glm::ivec3(-1, 0, 0),
        glm::ivec3(1, 0, 0),
    };
    for (int i = 0; i < offsetList.size(); i++) {
        if (isAirBlock(blockPos + offsetList[i])) return i;
    }
    return -1;
}

bool BlockUtils::isAirBlock(glm::ivec3 blockPos)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return false;
    int blockId = ClientInstance::get()->getBlockSource()->getBlock(blockPos)->toLegacy()->getBlockId();
    bool isLiquid = 8 <= blockId && blockId <= 11;
    return blockId == 0 || isLiquid;
}

glm::ivec3 BlockUtils::getClosestPlacePos(glm::ivec3 pos, float distance)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return pos;

    for (int i = 2; i < 6; i++) {
        glm::vec3 blockSel = pos + glm::ivec3(blockFaceOffsets[i]);
        if (isValidPlacePos(blockSel) && isAirBlock(blockSel)) return blockSel;
    }

    glm::vec3 playerPos = *player->getPos() - glm::vec3(0, PLAYER_HEIGHT + 1.f, 0);
    playerPos = glm::floor(playerPos);

    auto closestBlock = glm::vec3(INT_MAX, INT_MAX, INT_MAX);
    float closestDist = FLT_MAX;

    for (int x = pos.x - distance; x < pos.x + distance; x++)
        for (int y = pos.y - distance; y < pos.y + distance; y++)
            for (int z = pos.z - distance; z < pos.z + distance; z++)
            {
                auto blockSel = glm::vec3(x, y, z);
                if (isValidPlacePos(blockSel) && isAirBlock(blockSel)) {
                    float distance = glm::distance(playerPos, blockSel);
                    if (distance < closestDist) {
                        closestDist = distance;
                        closestBlock = blockSel;
                    }
                }
            }

    if (closestBlock.x != INT_MAX && closestBlock.y != INT_MAX && closestBlock.z != INT_MAX) {
        return closestBlock;
    }

    for (int i = 0; i < 1; i++) {
        glm::ivec3 blockSel = pos + glm::ivec3(blockFaceOffsets[i]);
        if (isValidPlacePos(blockSel)) return blockSel;
    }

    return { INT_MAX, INT_MAX, INT_MAX };
}

bool BlockUtils::isValidPlacePos(glm::ivec3 blockPos)
{
    return getBlockPlaceFace(blockPos) != -1;
}

void BlockUtils::placeBlock(glm::vec3 pos, int side)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    glm::ivec3 blockPos = pos;
    if (side == -1) side = getBlockPlaceFace(blockPos);

    glm::vec3 vec = blockPos;

    if (side != -1) vec += blockFaceOffsets[side] * 0.5f;

    HitResult* res = player->getLevel()->getHitResult();

    vec += blockFaceOffsets[side] * 0.5f;

    res->mBlockPos = vec;
    res->mFacing = side;

    res->mType = HitType::BLOCK;
    res->mIndirectHit = false;
    res->mRayDir = vec;
    res->mPos = blockPos;


    bool oldSwinging = player->isSwinging();
    int oldSwingProgress = player->getSwingProgress();
    player->getGameMode()->buildBlock(blockPos + glm::ivec3(blockFaceOffsets[side]) , side, true);
    player->setSwinging(oldSwinging);
    player->setSwingProgress(oldSwingProgress);

    vec += blockFaceOffsets[side] * 0.5f;

    res->mBlockPos = vec;
    res->mFacing = side;

    res->mType = HitType::BLOCK;
    res->mIndirectHit = false;
    res->mRayDir = vec;
    res->mPos = blockPos;
}

void BlockUtils::startDestroyBlock(glm::vec3 pos, int side)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    glm::ivec3 blockPos = pos;
    if (side == -1) side = getExposedFace(blockPos);

    glm::vec3 vec = blockPos;

    if (side != -1) vec += blockFaceOffsets[side] * -0.5f;

    HitResult* res = player->getLevel()->getHitResult();

    vec += blockFaceOffsets[side] * 0.5f;

    res->mBlockPos = vec;
    res->mFacing = side;

    res->mType = HitType::BLOCK;
    res->mIndirectHit = false;
    res->mRayDir = vec;
    res->mPos = blockPos;

    bool isDestroyedOut = false;
    player->getGameMode()->startDestroyBlock(&blockPos, side, isDestroyedOut);
    //if (!isDestroyedOut) player->getGameMode()->continueDestroyBlock(blockPos, side, *player->getPos(), isDestroyedOut);

    vec += blockFaceOffsets[side] * -0.5f;

    res->mBlockPos = vec;
    res->mFacing = side;

    res->mType = HitType::BLOCK;
    res->mIndirectHit = false;
    res->mRayDir = vec;
    res->mPos = blockPos;
}

void BlockUtils::clearBlock(const glm::ivec3& pos)
{
    Block* oldBlock = ClientInstance::get()->getBlockSource()->getBlock(pos);
    std::shared_ptr<UpdateBlockPacket> p = MinecraftPackets::createPacket<UpdateBlockPacket>();
    p->mPos = pos;
    p->mLayer = UpdateBlockPacket::BlockLayer::Standard;
    p->mUpdateFlags = BlockUpdateFlag::Priority;
    p->mBlockRuntimeId = 3690217760;
    PacketUtils::sendToSelf(p);
    Block* newBlock = ClientInstance::get()->getBlockSource()->getBlock(pos);

    // Fire a block update event because sendToSelf doesn't trigger it
    auto holder = nes::make_holder<BlockChangedEvent>(pos, newBlock, oldBlock, 0, ClientInstance::get()->getLocalPlayer());
    gFeatureManager->mDispatcher->trigger(holder);
}

void BlockUtils::destroyBlock(glm::vec3 pos, int side, bool useTransac)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    glm::ivec3 blockPos = pos;
    if (side == -1) side = getBlockPlaceFace(blockPos);

    if (!useTransac)
    {
        bool oldSwinging = player->isSwinging();
        int oldSwingProgress = player->getSwingProgress();

        player->getGameMode()->destroyBlock(&blockPos, side);
        player->getGameMode()->stopDestroyBlock(blockPos);
        player->setSwinging(oldSwinging);
        player->setSwingProgress(oldSwingProgress);
        return;
    }

    auto actionPkt = MinecraftPackets::createPacket<PlayerActionPacket>();
    actionPkt->mPos = blockPos;
    actionPkt->mResultPos = blockPos;
    actionPkt->mFace = side;
    actionPkt->mAction = PlayerActionType::StopDestroyBlock;
    actionPkt->mRuntimeId = player->getRuntimeID();
    actionPkt->mtIsFromServerPlayerMovementSystem = false;

    PacketUtils::queueSend(actionPkt);
    auto pkt = MinecraftPackets::createPacket<InventoryTransactionPacket>();

    auto cit = std::make_unique<ItemUseInventoryTransaction>();
    cit->actionType = ItemUseInventoryTransaction::ActionType::Destroy;
    int slot = player->getSupplies()->mSelectedSlot;
    cit->slot = slot;
    cit->itemInHand = *player->getSupplies()->getContainer()->getItem(slot);
    cit->blockPos = blockPos;
    cit->face = side;
    cit->targetBlockRuntimeId = 0;
    cit->playerPos = *player->getPos();

    BlockInfo block = BlockInfo(ClientInstance::get()->getBlockSource()->getBlock(blockPos), blockPos);
    AABB blockAABB = block.getAABB();

    cit->clickPos = blockAABB.getClosestPoint(*player->getPos());
    pkt->mTransaction = std::move(cit);
    PacketUtils::queueSend(pkt);

    // We broke the block, now we need to clear it manually
    // since were not using GameMode::destroyBlock which does it for us
    clearBlock(blockPos);

}
