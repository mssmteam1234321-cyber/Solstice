//
// 8/22/2024.
//

#include "AutoKick.hpp"

#include <Features/FeatureManager.hpp>
#include <Features/Events/BaseTickEvent.hpp>
#include <Features/Events/PacketOutEvent.hpp>
#include <Features/Events/PacketInEvent.hpp>
#include <Features/Events/PingUpdateEvent.hpp>
#include <Features/Events/SendImmediateEvent.hpp>
#include <Features/Modules/Combat/Aura.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Inventory/PlayerInventory.hpp>
#include <SDK/Minecraft/World/BlockSource.hpp>
#include <SDK/Minecraft/World/HitResult.hpp>
#include <SDK/Minecraft/World/Level.hpp>
#include <SDK/Minecraft/Network/Packets/PlayerAuthinputPacket.hpp>
#include <SDK/Minecraft/Network/Packets/UpdateBlockPacket.hpp>

void AutoKick::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &AutoKick::onBaseTickEvent, nes::event_priority::VERY_LAST>(this);
    gFeatureManager->mDispatcher->listen<PacketOutEvent, &AutoKick::onPacketOutEvent, nes::event_priority::VERY_LAST>(this);
    gFeatureManager->mDispatcher->listen<PacketInEvent, &AutoKick::onPacketInEvent>(this);
    gFeatureManager->mDispatcher->listen<SendImmediateEvent, &AutoKick::onSendImmediateEvent, nes::event_priority::VERY_LAST>(this);
    gFeatureManager->mDispatcher->listen<PingUpdateEvent, &AutoKick::onPingUpdateEvent, nes::event_priority::VERY_FIRST>(this);

    mLastTargetPos = { INT_MAX, INT_MAX, INT_MAX };
    mUsePrediction = false;
    mOnGroundTicks = 0;
}

void AutoKick::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &AutoKick::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketOutEvent, &AutoKick::onPacketOutEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketInEvent, &AutoKick::onPacketInEvent>(this);
    gFeatureManager->mDispatcher->deafen<SendImmediateEvent, &AutoKick::onSendImmediateEvent>(this);
    gFeatureManager->mDispatcher->deafen<PingUpdateEvent, &AutoKick::onPingUpdateEvent>(this);

    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;
    auto supplies = player->getSupplies();
    if (mSelectedSlot)
    {
        supplies->mSelectedSlot = mPreviousSlot;
        mSelectedSlot = false;
    }
    mShouldRotate = false;
    mRecentlyUpdatedBlockPositions.clear();
}

void AutoKick::onBaseTickEvent(BaseTickEvent& event)
{
    auto player = event.mActor;
    auto supplies = player->getSupplies();

    // Ghost checks
    for (auto pos : mRecentlyUpdatedBlockPositions) {
        if (BlockUtils::isAirBlock(pos)) {
            if (mDebug.mValue) ChatUtils::displayClientMessage("Failed to place block");
            if (mSpamPlace.mValue) mLastBlockPlace = 0;
        }
    }
    mRecentlyUpdatedBlockPositions.clear();

    //ChatUtils::displayClientMessage("Ping:" + std::to_string(mPing));
    if (mLastBlockPlace + mDelay.mValue > NOW) {
        if (mSelectedSlot) {
            supplies->mSelectedSlot = mPreviousSlot;
            mSelectedSlot = false;
        }
        mLastTargetPos = { INT_MAX, INT_MAX, INT_MAX };
        mUsePrediction = false;
        return;
    }

    if (!Aura::sHasTarget || !Aura::sTarget->getActorTypeComponent()) return;
    if (!Aura::sTarget->isPlayer()) return;

    if (ItemUtils::getAllPlaceables(mHotbarOnly.mValue) < 1) return;

    glm::vec3 targetPos = *Aura::sTarget->getPos();
    glm::vec3 prediction = { 0, 0, 0 };
    float PingTime = ((mPing / 50) * 2) + ((mOpponentPing.mValue / 50) * 2);
    float PredictedTime = PingTime + 2;
    if (mUsePrediction) {
        prediction = targetPos - mLastTargetPos;
    }
    if (!mAllowDiagonal.mValue) {
        if (abs(prediction.z) < abs(prediction.x)) prediction.z = 0;
        else prediction.x = 0;
    }
    glm::vec3 predictionPos = targetPos + glm::vec3(prediction.x * PingTime, 0, prediction.z * PingTime);
    glm::vec3 predictionBlockPos = targetPos + glm::vec3(prediction.x * PredictedTime, 0, prediction.z * PredictedTime);
    mUsePrediction = true;
    mLastTargetPos = targetPos;
    if (predictionPos == predictionBlockPos) return;
    targetPos = predictionPos;

    if (mOnGroundOnly.mValue) {
        if (prediction.y != 0) {
            return;
            mOnGroundTicks = 0;
        }
        mOnGroundTicks++;
        if (mOnGroundTicks < mMaxOnGroundTicks.mValue) return;
    }

    bool found = false;
    for (int i = 0; i <= 3; i++) {
        glm::ivec3 pos = glm::ivec3(floor(targetPos.x), floor(targetPos.y), floor(targetPos.z)) - glm::ivec3(0, i, 0);
        if (!BlockUtils::isAirBlock(pos + glm::ivec3(0, -1, 0))) {
            targetPos = pos;
            found = true;
            break;
        }
    }
    if (!found) return;

    int side = 1;
    int blockSlot = ItemUtils::getPlaceableItemOnBlock(targetPos, mHotbarOnly.mValue);
    if (blockSlot == -1) return;
    //if (BlockUtils::isAirBlock(targetPos + glm::vec3(0, -1, 0))) return;

    mPreviousSlot = supplies->mSelectedSlot;
    supplies->mSelectedSlot = blockSlot;
    mSelectedSlot = true;

    mCurrentPlacePos = targetPos;
    mShouldRotate = true;
    BlockUtils::placeBlock(targetPos, side);
    BlockUtils::placeBlock(targetPos + glm::vec3(0, 1, 0), side);
    mLastBlockPlace = NOW;
}

void AutoKick::onPacketOutEvent(PacketOutEvent& event) {
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

void AutoKick::onPacketInEvent(PacketInEvent& event) {
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    if (event.mPacket->getId() == PacketID::UpdateBlock) {
        auto pkt = event.getPacket<UpdateBlockPacket>();
        auto updateBlockPacket = pkt.get();
        if (updateBlockPacket->mPos == mCurrentPlacePos && !BlockUtils::isAirBlock(updateBlockPacket->mPos) && NOW - mLastBlockPlace <= 800) {
            mRecentlyUpdatedBlockPositions.push_back(updateBlockPacket->mPos);
        }
    }
}

void AutoKick::onSendImmediateEvent(SendImmediateEvent& event)
{
    uint8_t packetId = event.send[0];
    if (packetId == 0)
    {
        uint64_t timestamp = *reinterpret_cast<uint64_t*>(&event.send[1]);
        uint64_t timestamp64 = _byteswap_uint64(timestamp);
        uint64_t now = NOW;
        mEventDelay = now - timestamp64;
    }
}

void AutoKick::onPingUpdateEvent(PingUpdateEvent& event)
{
    mPing = event.mPing - mEventDelay;
}