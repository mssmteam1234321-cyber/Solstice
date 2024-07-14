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
#include <SDK/Minecraft/Network/Packets/MobEquipmentPacket.hpp>
#include <SDK/Minecraft/World/BlockLegacy.hpp>

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
    mShouldSpoofSlot = true;
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
    AABB blockAABB = AABB(blockPos, glm::vec3(1, 1, 1));
    glm::vec3 closestPos = blockAABB.getClosestPoint(*player->getPos());
    if (mRange.mValue < glm::distance(closestPos, *player->getPos())) return false;

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
    int bestToolSlot = ItemUtils::getBestBreakingTool(block, mHotbarOnly.mValue);
    BlockUtils::startDestroyBlock(blockPos, mCurrentBlockFace);
    if (mShouldSpoofSlot) {
        PacketUtils::spoofSlot(bestToolSlot);
        mShouldSpoofSlot = false;
    }
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

    float absorption = player->getAbsorption();
    bool maxAbsorption = 10 <= absorption;

    // Return if maxAbsorption is reached, OR if a block was placed in the last 200ms
    if (maxAbsorption && !mQueueRedstone.mValue || mLastBlockPlace + 500 > NOW) {
        initializeRegen();
        return;
    }

    if (isValidBlock(mCurrentBlockPos, !mUncover, !mIsUncovering)) { // Check if current block is valid
        Block* currentBlock = source->getBlock(mCurrentBlockPos);
        int bestToolSlot = ItemUtils::getBestBreakingTool(currentBlock, mHotbarOnly.mValue);
        if (mShouldSpoofSlot) {
            PacketUtils::spoofSlot(bestToolSlot);
            mShouldSpoofSlot = false;
        }
        mToolSlot = bestToolSlot;

        float destroySpeed = ItemUtils::getDestroySpeed(bestToolSlot, currentBlock);

        if (!mOldCalculation.mValue) mBreakingProgress += ItemUtils::getDestroySpeed(bestToolSlot, currentBlock);
        else mBreakingProgress += ItemUtils::getDestroySpeed(bestToolSlot, currentBlock, mDestroySpeed.mValue);

        bool isRedstone = currentBlock->getmLegacy()->getBlockId() == 73 || currentBlock->getmLegacy()->getBlockId() == 74;

        bool finishBreak = true;
        if (maxAbsorption && isRedstone) finishBreak = false;

        if ((mDestroySpeed.mValue <= mBreakingProgress && !mOldCalculation.mValue  || 1 <= mBreakingProgress && mOldCalculation.mValue) && finishBreak) {
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

        bool foundBlock = false;
        glm::vec3 playerPos = *player->getPos();
        glm::ivec3 pos;
        glm::ivec3 targettingPos;
        float closestDistance = INT_MAX;
        if (!exposedBlockList.empty()) {
            for (int i = 0; i < exposedBlockList.size(); i++) {
                glm::vec3 blockPos = exposedBlockList[i].mPosition;
                float dist = glm::distance(playerPos, blockPos);
                if (dist < closestDistance) {
                    closestDistance = dist;
                    pos = blockPos;
                    targettingPos = blockPos;
                    foundBlock = true;
                }
            }
            // queue block
            if (foundBlock) {
                mTargettingBlockPos = targettingPos;
                queueBlock(pos);
                return;
            }
        }
        else if (mUncover && !unexposedBlockList.empty()) {
            for (int i = 1; i <= 2; i++) {
                for (int i2 = 0; i2 < unexposedBlockList.size(); i2++) {
                    glm::ivec3 blockPos = unexposedBlockList[i2].mPosition;
                    blockPos.y += i;
                    if (BlockUtils::getExposedFace(blockPos) != -1 || i == 2) { // Is exposed
                        float dist = glm::distance(playerPos, glm::vec3(blockPos));
                        if (dist < closestDistance) {
                            closestDistance = dist;
                            pos = blockPos;
                            targettingPos = blockPos;
                            foundBlock = true;
                        }
                    }
                }
                // queue block
                if (foundBlock) {
                    mIsUncovering = true;
                    mTargettingBlockPos = targettingPos;
                    queueBlock(pos);
                    return;
                }
            }
        }
    }
}


void Regen::onRenderEvent(RenderEvent& event)
{
    // NOTE: Keep in mind if you have a RenderEvent that needs something to animate out when disabled,
    //       you need to put the listen call in the module's constructor without any deafen call.
    // TODO: Regen progress bar

    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    if (mRenderBlock.mValue && mIsMiningBlock)
    {
        static float lastProgress = 0.f;
        float progress = 1.f;

        progress = mBreakingProgress;
        if (!mOldCalculation.mValue) progress /= mDestroySpeed.mValue;
        if (progress < lastProgress) lastProgress = progress;
        progress = MathUtils::lerp(lastProgress, progress, ImGui::GetIO().DeltaTime * 30.f);
        lastProgress = progress;

        // clamp the progress to 0-1
        progress = MathUtils::clamp(progress, 0.f, 1.f);

        if (progress < 0.01f) return;


        auto size = glm::vec3(progress, progress, progress);
        glm::vec3 blockPos = mCurrentBlockPos;
        blockPos.x += 0.5f - (progress / 2.f);
        blockPos.y += 0.5f - (progress / 2.f);
        blockPos.z += 0.5f - (progress / 2.f);
        auto blockAABB = AABB(blockPos, size);
        RenderUtils::drawOutlinedAABB(blockAABB, ColorUtils::getThemedColor(0)); // TODO: Replace the color with the color of the progress bar as per state (queueing, uncovering, mining, stealing, etc.)
    }

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
            const glm::vec3 blockPos = mCurrentBlockPos;
            auto blockAABB = AABB(blockPos, glm::vec3(1, 1, 1));
            glm::vec2 rotations = MathUtils::getRots(*player->getPos(), blockAABB);
            paip->mRot = rotations;
            paip->mYHeadRot = rotations.y;
            mShouldRotate = false;
        }
    }else if (event.packet->getId() == PacketID::InventoryTransaction) {
        if (const auto it = event.getPacket<InventoryTransactionPacket>();
            it->mTransaction->type == ComplexInventoryTransaction::Type::ItemUseTransaction)
        {
            if (const auto transac = reinterpret_cast<ItemUseInventoryTransaction*>(it->mTransaction.get());
                transac->actionType == ItemUseInventoryTransaction::ActionType::Place)
            {
                mLastBlockPlace = NOW;
            }
        }
    }
    else if (event.packet->getId() == PacketID::MobEquipment) {
        auto mpkt = event.getPacket<MobEquipmentPacket>();
        if (mpkt->mSlot != mToolSlot) mShouldSpoofSlot = true;
    }
}