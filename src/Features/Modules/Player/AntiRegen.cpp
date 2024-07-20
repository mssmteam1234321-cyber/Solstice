#include "AntiRegen.hpp"

// to get targetting block
#include "Regen.hpp"

#include <Features/FeatureManager.hpp>
#include <Features/Events/BaseTickEvent.hpp>
#include <Features/Events/PacketInEvent.hpp>
#include <Features/Events/PacketOutEvent.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Inventory/PlayerInventory.hpp>
#include <SDK/Minecraft/Actor/GameMode.hpp>
#include <SDK/Minecraft/Network/PacketID.hpp>
#include <SDK/Minecraft/Network/Packets/PlayerAuthInputPacket.hpp>
#include <SDK/Minecraft/Network/Packets/LevelEventPacket.hpp>
#include <SDK/Minecraft/World/BlockSource.hpp>
#include <SDK/Minecraft/World/BlockLegacy.hpp>
#include <SDK/Minecraft/World/Block.hpp>
#include <SDK/Minecraft/World/Level.hpp>
#include <SDK/Minecraft/World/HitResult.hpp>
#include <Utils/GameUtils/ItemUtils.hpp>
#include <Utils/GameUtils/PacketUtils.hpp>
#include <Utils/MiscUtils/BlockUtils.hpp>

bool AntiRegen::isValidRedstone(glm::ivec3 blockPos) {
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return false;

    int blockID = ClientInstance::get()->getBlockSource()->getBlock(blockPos)->getmLegacy()->getBlockId();
    static Regen* regenModule = gFeatureManager->mModuleManager->getModule<Regen>();

    // BlockID Check
    if (blockID != 73 && blockID != 74) return false;

    // BlockPos Check
    if (mRange.mValue < glm::distance(*player->getPos(), glm::vec3(blockPos)) || blockPos == regenModule->mTargettingBlockPos) return false;

    // Hit Result Check
    if (player->getLevel()->getHitResult()->mBlockPos == blockPos) return false;

    return true;
}

void AntiRegen::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &AntiRegen::onBaseTickEvent, nes::event_priority::LAST>(this);
    gFeatureManager->mDispatcher->listen<PacketInEvent, &AntiRegen::onPacketInEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketOutEvent, &AntiRegen::onPacketOutEvent, nes::event_priority::LAST>(this);

    miningRedstones.clear();
    mPreviousSlot = -1;
    mPlacedBlock = false;
    mShouldRotate = false;
    
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;
}

void AntiRegen::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &AntiRegen::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketInEvent, &AntiRegen::onPacketInEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketOutEvent, &AntiRegen::onPacketOutEvent>(this);

    miningRedstones.clear();
    
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;
    PlayerInventory* supplies = player->getSupplies();

    if (mPlacedBlock) supplies->mSelectedSlot = mPreviousSlot;
    mPreviousSlot = -1;
    mPlacedBlock = false;
    mShouldRotate = false;
}

void AntiRegen::onBaseTickEvent(BaseTickEvent& event)
{
    auto player = event.mActor;
    PlayerInventory* supplies = player->getSupplies();
    static Regen* regenModule = gFeatureManager->mModuleManager->getModule<Regen>();

    if (mMode.as<Mode>() == Mode::Cover) {
        if (regenModule->mEnabled && regenModule->mShouldRotate) return;

        if (0 < ItemUtils::getAllPlaceables(mHotbarOnly.mValue)) {
            for (auto& pos : miningRedstones)
            {
                int exposedFace = BlockUtils::getExposedFace(pos);
                if (exposedFace == -1 || !isValidRedstone(pos)) continue;
                glm::ivec3 placePos = pos + offsetList[exposedFace];
                glm::ivec3 hitPos = placePos + glm::ivec3(0, -1, 0);
                if (ClientInstance::get()->getBlockSource()->getBlock(placePos + glm::ivec3(0, -1, 0))->getmLegacy()->isAir()) continue;
                mCurrentPlacePos = placePos;
                mShouldRotate = true;
                mPreviousSlot = supplies->mSelectedSlot;
                int blockSlot = ItemUtils::getPlaceableItemOnBlock(placePos, mHotbarOnly.mValue, false);

                supplies->mSelectedSlot = blockSlot;
                PacketUtils::spoofSlot(blockSlot);
                if (mSwing.mValue) player->swing();

                //PlaceBlock without flagged by anticheat
                HitResult* res = player->getLevel()->getHitResult();
                bool oldSwinging = player->isSwinging();
                int oldSwingProgress = player->getSwingProgress();
                res->mBlockPos = placePos;
                res->mFacing = 1;
                res->mType = HitType::BLOCK;
                res->mIndirectHit = true;
                res->mRayDir = placePos;
                res->mPos = placePos;
                player->getGameMode()->buildBlock(hitPos, 1, true);
                player->setSwinging(oldSwinging);
                player->setSwingProgress(oldSwingProgress);
                mPlacedBlock = true;
                supplies->mSelectedSlot = mPreviousSlot;
                return;
            }
        }

        miningRedstones.clear();

        if (mPlacedBlock) {
            if(!regenModule->mEnabled || !regenModule->mIsMiningBlock) PacketUtils::spoofSlot(mPreviousSlot);
            mPlacedBlock = false;
        }
    }
}

void AntiRegen::onPacketOutEvent(PacketOutEvent& event)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    if (event.mPacket->getId() == PacketID::PlayerAuthInput)
    {
        auto paip = event.getPacket<PlayerAuthInputPacket>();
        if (mShouldRotate)
        {
            const glm::vec3 blockPos = mCurrentPlacePos;
            auto blockAABB = AABB(blockPos, glm::vec3(1, 1, 1));
            glm::vec2 rotations = MathUtils::getRots(*player->getPos(), blockAABB);
            paip->mRot = rotations;
            paip->mYHeadRot = rotations.y;
            mShouldRotate = false;
        }
    }
}

void AntiRegen::onPacketInEvent(PacketInEvent& event) {
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    if (event.mPacket->getId() == PacketID::LevelEvent) {
        auto levelEvent = event.getPacket<LevelEventPacket>();
        if (levelEvent->mEventId == 3600) { // Start destroying block
            if (!isValidRedstone(levelEvent->mPos)) {
                return;
            }
            miningRedstones.push_back(levelEvent->mPos);
        }
    }
}