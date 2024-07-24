#include "Regen.hpp"

#include <Features/FeatureManager.hpp>
#include <Features/Events/BaseTickEvent.hpp>
#include <Features/Events/PacketOutEvent.hpp>
#include <Features/Events/PacketInEvent.hpp>
#include <Features/Modules/Combat/Aura.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Inventory/PlayerInventory.hpp>
#include <SDK/Minecraft/Actor/GameMode.hpp>
#include <SDK/Minecraft/Network/PacketID.hpp>
#include <SDK/Minecraft/Network/Packets/InventoryTransactionPacket.hpp>
#include <SDK/Minecraft/Network/Packets/PlayerAuthInputPacket.hpp>
#include <SDK/Minecraft/Network/Packets/MobEquipmentPacket.hpp>
#include <SDK/Minecraft/Network/Packets/LevelEventPacket.hpp>
#include <SDK/Minecraft/World/BlockLegacy.hpp>
#include <SDK/Minecraft/World/Level.hpp>
#include <SDK/Minecraft/World/HitResult.hpp>

void Regen::initializeRegen() {
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    GameMode* gm = player->getGameMode();

    if (mIsMiningBlock) {
        gm->stopDestroyBlock(mCurrentBlockPos);
    }
    mCurrentBlockPos = { 0, 0, 0 };
    mTargettingBlockPos = { 0, 0, 0 };
    mCurrentBlockFace = -1;
    mBreakingProgress = 0.f;
    mShouldSpoofSlot = true;
    mIsMiningBlock = false;
    mIsUncovering = false;
    mIsStealing = false;
    mToolSlot = -1;
}

bool Regen::isValidBlock(glm::ivec3 blockPos, bool redstoneOnly, bool exposedOnly, bool isStealing) {
    auto player = ClientInstance::get()->getLocalPlayer();
    Block* block = ClientInstance::get()->getBlockSource()->getBlock(blockPos);
    if (!block) return false;

    // Air Check
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
    int exposedFace = BlockUtils::getExposedFace(blockPos);
    if (exposedOnly && (!isStealing || !isRedstone)) {
        if (exposedFace == -1) return false;
    }

    // Steal
    if (isStealing && !mCanSteal && exposedFace == -1) return false;

    // Anti Steal
    if (mAntiSteal.mValue && blockPos == mBlackListedOrePos && exposedFace == -1) return false;

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
    if (mShouldSpoofSlot) {
        PacketUtils::spoofSlot(bestToolSlot);
        mShouldSpoofSlot = false;
    }
    BlockUtils::startDestroyBlock(blockPos, mCurrentBlockFace);
    mToolSlot = bestToolSlot;
    mShouldSetbackSlot = true;
    //mBreakingProgress += ItemUtils::getDestroySpeed(bestToolSlot, block);
}

Regen::PathFindingResult Regen::getBestPathToBlock(glm::ivec3 blockPos) {
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return { glm::ivec3(0, 0, 0), 0 };

    BlockSource* source = ClientInstance::get()->getBlockSource();
    if (!source) return { glm::ivec3(0, 0, 0), 0 };

    static std::vector<glm::ivec3> offsetList = {
        glm::ivec3(0, 1, 0),
        glm::ivec3(0, 0, -1),
        glm::ivec3(0, 0, 1),
        glm::ivec3(-1, 0, 0),
        glm::ivec3(1, 0, 0),
    };

    float currentBreakingTime = 0;
    float bestBreakingTime = INT_MAX;
    glm::ivec3 bestPos = { 0, 0, 0 };

    for (int i = 0; i < 5; i++) {
        currentBreakingTime = 0;
        glm::ivec3 pos1 = blockPos + offsetList[i];
        Block* currentBlock = source->getBlock(pos1);
        bool isAir = currentBlock->getmLegacy()->isAir();
        if (!isAir) currentBreakingTime += 1 / ItemUtils::getDestroySpeed(ItemUtils::getBestBreakingTool(currentBlock, mHotbarOnly.mValue), currentBlock);
        for (int i2 = 0; i2 < 5; i2++) {
            if (-offsetList[i] == offsetList[i2]) continue;
            glm::ivec3 pos2 = pos1 + offsetList[i2];
            Block* currentBlock2 = source->getBlock(pos2);
            bool isAir2 = currentBlock2->getmLegacy()->isAir();
            if (!isAir2) currentBreakingTime += 1 / ItemUtils::getDestroySpeed(ItemUtils::getBestBreakingTool(currentBlock2, mHotbarOnly.mValue), currentBlock2);
            if (currentBreakingTime < bestBreakingTime) {
                bestBreakingTime = currentBreakingTime;
                if (!isAir2) bestPos = pos2;
                else bestPos = pos1;
            }
        }
    }

    return { bestPos, bestBreakingTime };
}

void Regen::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &Regen::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->listen<RenderEvent, &Regen::onRenderEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketOutEvent, &Regen::onPacketOutEvent, nes::event_priority::VERY_LAST>(this);
    gFeatureManager->mDispatcher->listen<PacketInEvent, &Regen::onPacketInEvent>(this);

    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    mShouldRotate = false;
    mIsMiningBlock = false;
    mEnemyTargettingBlockPos = { 0, 0, 0 };
    initializeRegen();
}

void Regen::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &Regen::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->deafen<RenderEvent, &Regen::onRenderEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketOutEvent, &Regen::onPacketOutEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketInEvent, &Regen::onPacketInEvent>(this);

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
    bool steal = mSteal.mValue && (mCanSteal || mIsStealing);

    // Return if maxAbsorption is reached, OR if a block was placed in the last 200ms
    if (maxAbsorption && !mQueueRedstone.mValue && (!mAlwaysSteal.mValue || !steal)) {
        initializeRegen();
        if (mIsConfuserActivated) {
            player->getGameMode()->stopDestroyBlock(mLastConfusedPos);
            mIsConfuserActivated = false;
        }

        if (mShouldSetbackSlot) {
            PacketUtils::spoofSlot(mPreviousSlot);
            mShouldSetbackSlot = false;
        }
        return;
    }

    // Return without reset breaking progress
    if (mLastBlockPlace + 100 > NOW) {
        if (mIsMiningBlock) PacketUtils::spoofSlot(mLastPlacedBlockSlot);
        return;
    }

    if (isValidBlock(mCurrentBlockPos, !mUncover, !mIsUncovering, mIsStealing) && mTargettingBlockPos != mBlackListedOrePos) { // Check if current block is valid
        Block* currentBlock = source->getBlock(mCurrentBlockPos);
        int exposedFace = BlockUtils::getExposedFace(mCurrentBlockPos);
        int bestToolSlot = ItemUtils::getBestBreakingTool(currentBlock, mHotbarOnly.mValue);
        if (mShouldSpoofSlot) {
            PacketUtils::spoofSlot(bestToolSlot);
            mShouldSpoofSlot = false;
        }
        mToolSlot = bestToolSlot;

        bool isRedstone = currentBlock->getmLegacy()->getBlockId() == 73 || currentBlock->getmLegacy()->getBlockId() == 74;

        float destroySpeed = ItemUtils::getDestroySpeed(bestToolSlot, currentBlock);
        if (mCalcMode.mValue == CalcMode::Normal) {
            if (isRedstone) mCurrentDestroySpeed = mDestroySpeed.mValue;
            else mCurrentDestroySpeed = mOtherDestroySpeed.mValue;
        }
        else if (mCalcMode.mValue == CalcMode::Dynamic) {
            std::string blockName = currentBlock->getmLegacy()->getmName();
            bool found = false;
            for (auto& c : dynamicSpeeds) {
                if (c.blockName == blockName) {
                    mCurrentDestroySpeed = c.destroySpeed;
                    found = true;
                    break;
                }
            }
            if (!found) {
                if (isRedstone) mCurrentDestroySpeed = mDestroySpeed.mValue;
                else mCurrentDestroySpeed = mOtherDestroySpeed.mValue;
            }
        }
        

        if (!mOldCalculation.mValue) mBreakingProgress += destroySpeed;
        else mBreakingProgress += ItemUtils::getDestroySpeed(bestToolSlot, currentBlock, mCurrentDestroySpeed);

        bool finishBreak = true;
        if (maxAbsorption && isRedstone && !mIsStealing) finishBreak = false;

        if ((mCurrentDestroySpeed <= mBreakingProgress && (!mIsStealing || exposedFace != -1) && !mOldCalculation.mValue || 1 <= mBreakingProgress && mOldCalculation.mValue) && finishBreak) {
            mShouldRotate = true;
            supplies->mSelectedSlot = bestToolSlot;
            if (mSwing.mValue) player->swing();
            BlockUtils::destroyBlock(mCurrentBlockPos, exposedFace);
            supplies->mSelectedSlot = mPreviousSlot;
            mIsMiningBlock = false;
            return;
        }
    }
    else { // Find new block
        initializeRegen();
        if (mCanSteal && mSteal.mValue && isValidBlock(mEnemyTargettingBlockPos, true, false)) {
            queueBlock(mEnemyTargettingBlockPos);
            mTargettingBlockPos = mEnemyTargettingBlockPos;
            mIsStealing = true;
            return;
        }
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

        glm::vec3 playerPos = *player->getPos();
        glm::ivec3 pos = { 0, 0, 0 };
        glm::ivec3 targettingPos = { 0, 0, 0 };
        if (!exposedBlockList.empty()) {
            // Confuser
            if (mConfuse.mValue) {
                if (mIsConfuserActivated) {
                    player->getGameMode()->stopDestroyBlock(mLastConfusedPos);
                    mIsConfuserActivated = false;
                }
                else if(!unexposedBlockList.empty()) {
                    glm::ivec3 confusePos = unexposedBlockList[0].mPosition + glm::ivec3(0, -1, 0);
                    BlockUtils::startDestroyBlock(confusePos, 0);
                    mLastConfusedPos = confusePos;
                    mIsConfuserActivated = true;
                    return;
                }
            }

            float closestDistance = INT_MAX;
            for (int i = 0; i < exposedBlockList.size(); i++) {
                glm::vec3 blockPos = exposedBlockList[i].mPosition;
                float dist = glm::distance(playerPos, blockPos);
                if (dist < closestDistance) {
                    closestDistance = dist;
                    pos = blockPos;
                    targettingPos = blockPos;
                }
            }
            // queue block
            queueBlock(pos);
            mTargettingBlockPos = targettingPos;
            return;
        }
        else if (mUncover && !unexposedBlockList.empty()) {
            bool foundBlock = false;
            bool isNextToRedstone = false;
            float fastestTime = INT_MAX;
            for (int i = 0; i < unexposedBlockList.size(); i++) {
                glm::ivec3 redstonePos = unexposedBlockList[i].mPosition;
                PathFindingResult result = getBestPathToBlock(redstonePos);
                float currentTime = result.time;
                if (currentTime >= fastestTime || !isValidBlock(result.blockPos, false, false)) continue;

                for (auto& [face, offset] : BlockUtils::blockFaceOffsets) {
                    int ID = source->getBlock(result.blockPos + glm::ivec3(offset))->getmLegacy()->getBlockId();
                    if (ID == 73 || ID == 74) {
                        isNextToRedstone = true;
                        break;
                    }
                }

                if (BlockUtils::getExposedFace(result.blockPos) == -1 && isNextToRedstone) continue;

                fastestTime = currentTime;
                pos = result.blockPos;
                targettingPos = redstonePos;
                foundBlock = true;
            }
            if (foundBlock) {
                queueBlock(pos);
                mIsUncovering = true;
                mTargettingBlockPos = targettingPos;
                return;
            }
        }

        if (mShouldSetbackSlot) {
            PacketUtils::spoofSlot(mPreviousSlot);
            mShouldSetbackSlot = false;
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
        if (!mOldCalculation.mValue) progress /= mCurrentDestroySpeed;
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

    if (event.mPacket->getId() == PacketID::PlayerAuthInput)
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
    }
    else if (event.mPacket->getId() == PacketID::InventoryTransaction) {
        if (const auto it = event.getPacket<InventoryTransactionPacket>();
            it->mTransaction->type == ComplexInventoryTransaction::Type::ItemUseTransaction)
        {
            if (const auto transac = reinterpret_cast<ItemUseInventoryTransaction*>(it->mTransaction.get());
                transac->actionType == ItemUseInventoryTransaction::ActionType::Place)
            {
                mLastBlockPlace = NOW;
                mLastPlacedBlockSlot = transac->slot;
            }
        }
    }
    else if (event.mPacket->getId() == PacketID::MobEquipment) {
        auto mpkt = event.getPacket<MobEquipmentPacket>();
        if (mpkt->mSlot != mToolSlot) mShouldSpoofSlot = true;
    }
}

void Regen::onPacketInEvent(class PacketInEvent& event) {
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    if (event.mPacket->getId() == PacketID::LevelEvent) {
        auto levelEvent = event.getPacket<LevelEventPacket>();
        if (levelEvent->mEventId == 3600) { // Start destroying block
            if (player->getLevel()->getHitResult()->mBlockPos == glm::ivec3(levelEvent->mPos) && 0 < player->getGameMode()->mBreakProgress) return;
            // Steal
            for (auto& offset : offsetList) {
                glm::ivec3 blockPos = glm::ivec3(levelEvent->mPos) + offset;
                if (isValidBlock(blockPos, true, false) && BlockUtils::getExposedFace(blockPos) == -1 && blockPos != mTargettingBlockPos) {
                    mEnemyTargettingBlockPos = blockPos;
                    mLastEnemyLayerBlockPos = levelEvent->mPos;
                    mCanSteal = true;
                }
            }

            // Anti Steal
            glm::ivec3 pos = glm::ivec3(levelEvent->mPos);
            if (pos == mTargettingBlockPos && pos != mCurrentBlockPos) {
                if (mAntiSteal.mValue) {
                    mBlackListedOrePos = pos;
                }
            }
        }
        else if (levelEvent->mEventId == 3601) { // Stop destroying block
            if (mCanSteal && glm::ivec3(levelEvent->mPos) == mLastEnemyLayerBlockPos) {
                mCanSteal = false;
            }

            if (glm::ivec3(levelEvent->mPos) == mBlackListedOrePos) {
                if (mAntiSteal.mValue) {
                    mBlackListedOrePos = { INT_MAX, INT_MAX, INT_MAX };
                }
            }
        }
    }
}