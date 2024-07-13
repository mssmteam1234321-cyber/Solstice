#include "Regen.hpp"

#include <Features/FeatureManager.hpp>
#include <Features/Events/BaseTickEvent.hpp>
#include <Features/Events/PacketOutEvent.hpp>
#include <Features/Modules/Combat/Aura.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Inventory/PlayerInventory.hpp>
#include <SDK/Minecraft/Actor/GameMode.hpp>
#include <SDK/Minecraft/Network/PacketID.hpp>
#include <SDK/Minecraft/Network/Packets/InventoryTransactionPacket.hpp>
#include <SDK/Minecraft/Network/Packets/PlayerAuthInputPacket.hpp>
#include <Utils/GameUtils/ItemUtils.hpp>
#include <Utils/GameUtils/PacketUtils.hpp>
#include <Utils/MiscUtils/BlockUtils.hpp>
#include <Utils/MiscUtils/ColorUtils.hpp>
#include <Utils/MiscUtils/MathUtils.hpp>

void Regen::initializeRegen() {
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    GameMode* gm = player->getGameMode();

    if (mIsMiningBlock) {
        PacketUtils::spoofSlot(mPreviousSlot);
        gm->stopDestroyBlock(mCurrentBlockPos);
    }
    mCurrentBlockPos = { 0, 0, 0 };
    mTargettingBlockPos = { 0, 0, 0 };
    mCurrentBlockFace = -1;
    mBreakingProgress = 0.f;
    mIsMiningBlock = false;
    mIsUncovering = false;
    mToolSlot = -1;
}

bool Regen::isValidBlock(glm::ivec3 blockPos, bool redstoneOnly, bool exposedOnly) {
    auto player = ClientInstance::get()->getLocalPlayer();
    Block* block = ClientInstance::get()->getBlockSource()->getBlock(blockPos);
    if (!block) return false;


    //
    if (block->getmLegacy()->isAir()) return false;

    // BlockID Check
    int blockId = block->getmLegacy()->getBlockId();
    bool isRedstone = blockId == 73 || blockId == 74;
    if (redstoneOnly) {
        if (!isRedstone) return false;
    }

    // Distance Check
    if (mRange.mValue < glm::distance(*player->getPos(), glm::vec3(blockPos))) return false;

    // Exposed Check
    if (exposedOnly) {
        if (BlockUtils::getExposedFace(blockPos) == -1) return false;
    }

    return true;
}

void Regen::queueBlock(glm::ivec3 blockPos) {
    Block* block = ClientInstance::get()->getBlockSource()->getBlock(blockPos);
    mCurrentBlockPos = blockPos;
    mCurrentBlockFace = BlockUtils::getExposedFace(blockPos);
    if (mCurrentBlockFace == -1) mCurrentBlockFace = 0;
    mIsMiningBlock = true;
    mBreakingProgress = 0.f;
    int bestToolSlot = ItemUtils::getBestBreakingTool(block);
    BlockUtils::startDestroyBlock(blockPos, mCurrentBlockFace);
    PacketUtils::spoofSlot(bestToolSlot);
    mToolSlot = bestToolSlot;
    //mBreakingProgress += ItemUtils::getDestroySpeed(bestToolSlot, block);
}

void Regen::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &Regen::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->listen<RenderEvent, &Regen::onRenderEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketOutEvent, &Regen::onPacketOutEvent, nes::event_priority::VERY_LAST>(this);

    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    mShouldRotate = false;
    mIsMiningBlock = false;
    initializeRegen();
}

void Regen::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &Regen::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->deafen<RenderEvent, &Regen::onRenderEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketOutEvent, &Regen::onPacketOutEvent>(this);

    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    if (mIsMiningBlock) {
        player->getSupplies()->mSelectedSlot = mToolSlot;
        player->getGameMode()->stopDestroyBlock(mCurrentBlockPos);
    }
}

void Regen::onBaseTickEvent(BaseTickEvent& event)
{
    auto player = event.mActor;
    BlockSource* source = ClientInstance::get()->getBlockSource();
    if (!source) return;
    PlayerInventory* supplies = player->getSupplies();
    mPreviousSlot = supplies->getmSelectedSlot();

    /*float absorption = 0;
    if (10 <= absorption) {
        initializeRegen();
        return;
    }*/

    if (isValidBlock(mCurrentBlockPos, !mUncover, !mIsUncovering)) { // Check if current block is valid
        Block* currentBlock = source->getBlock(mCurrentBlockPos);
        int bestToolSlot = ItemUtils::getBestBreakingTool(currentBlock);
        PacketUtils::spoofSlot(bestToolSlot);
        mToolSlot = bestToolSlot;
        mBreakingProgress += ItemUtils::getDestroySpeed(bestToolSlot, currentBlock);

        if (mDestroySpeed.mValue <= mBreakingProgress) { // Destroy block
            mShouldRotate = true;
            supplies->mSelectedSlot = bestToolSlot;
            if (mSwing.mValue) player->swing();
            BlockUtils::destroyBlock(mCurrentBlockPos, BlockUtils::getExposedFace(mCurrentBlockPos));
            supplies->mSelectedSlot = mPreviousSlot;
            mIsMiningBlock = false;
            return;
        }
    }
    else if (mIsMiningBlock) { // Lost redstone
        initializeRegen();
        return;
    }
    else { // Find new block
        initializeRegen();
        std::vector<BlockInfo> blockList = BlockUtils::getBlockList(*player->getPos(), mRange.mValue);
        std::vector<BlockInfo> exposedBlockList;
        std::vector<BlockInfo> unexposedBlockList;

        for (int i = 0; i < blockList.size(); i++) {
            if (isValidBlock(blockList[i].mPosition, true, false)) {
                if (BlockUtils::getExposedFace(blockList[i].mPosition) != -1) exposedBlockList.push_back(blockList[i]);
                else unexposedBlockList.push_back(blockList[i]);
            }
            else continue;
        }

        if (!exposedBlockList.empty()) {
            for (int i = 0; i < exposedBlockList.size(); i++) {
                mTargettingBlockPos = exposedBlockList[i].mPosition;
                queueBlock(exposedBlockList[i].mPosition);
                return;
            }
        }
        else if (mUncover && !unexposedBlockList.empty()) {
            for (int i = 1; i <= 2; i++) {
                for (int i2 = 0; i2 < unexposedBlockList.size(); i2++) {
                    glm::ivec3 blockPos = unexposedBlockList[i2].mPosition;
                    blockPos.y += i;
                    if (BlockUtils::getExposedFace(blockPos) != -1 || i == 2) { // Is exposed
                        mIsUncovering = true;
                        mTargettingBlockPos = unexposedBlockList[i2].mPosition;
                        queueBlock(blockPos);
                        return;
                    }
                }
            }
        }
    }
}


void Regen::onRenderEvent(RenderEvent& event)
{
    // TODO
    // render slider code
}

void Regen::onPacketOutEvent(PacketOutEvent& event)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    if (event.packet->getId() == PacketID::PlayerAuthInput)
    {
        auto paip = event.getPacket<PlayerAuthInputPacket>();

        if (mShouldRotate)
        {
            glm::vec3 blockPos = glm::vec3(mCurrentBlockPos.x + 0.5f, mCurrentBlockPos.y + 0.5f, mCurrentBlockPos.z + 0.5f);
            glm::vec3 side = BlockUtils::blockFaceOffsets[mCurrentBlockFace] * -0.5f;
            glm::vec3 target = blockPos + side;
            glm::vec2 rotations = MathUtils::getRots(*player->getPos(), target);
            paip->mRot = rotations;
            paip->mYHeadRot = rotations.y;
            mShouldRotate = false;
        }
    }
}