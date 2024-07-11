//
// Created by vastrakai on 7/10/2024.
//

#include "Scaffold.hpp"

#include <cmake-build-release/_deps/sdl2-build/include/SDL2/SDL_stdinc.h>
#include <Features/FeatureManager.hpp>
#include <Features/Events/BaseTickEvent.hpp>
#include <Features/Events/PacketOutEvent.hpp>
#include <Features/Modules/Combat/Aura.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Inventory/PlayerInventory.hpp>
#include <SDK/Minecraft/Network/PacketID.hpp>
#include <SDK/Minecraft/Network/Packets/InventoryTransactionPacket.hpp>
#include <SDK/Minecraft/Network/Packets/PlayerAuthInputPacket.hpp>
#include <Utils/Keyboard.hpp>
#include <Utils/GameUtils/ItemUtils.hpp>
#include <Utils/MiscUtils/BlockUtils.hpp>
#include <Utils/MiscUtils/ColorUtils.hpp>
#include <Utils/MiscUtils/MathUtils.hpp>

void Scaffold::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &Scaffold::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->listen<RenderEvent, &Scaffold::onRenderEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketOutEvent, &Scaffold::onPacketOutEvent, nes::event_priority::VERY_LAST>(this);


    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    mStartY = player->getPos()->y - PLAYER_HEIGHT - 1.f;
}

void Scaffold::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &Scaffold::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->deafen<RenderEvent, &Scaffold::onRenderEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketOutEvent, &Scaffold::onPacketOutEvent>(this);

    // Reset fields
    mStartY = 0.f;
    mLastBlock = {0, 0, 0};
    mLastFace = 0;
    mLastSwitchTime = 0;
    mShouldRotate = false;
}

void Scaffold::onBaseTickEvent(BaseTickEvent& event)
{
    auto player = event.mActor;

    for (int i = 0; i < mPlaces.as<int>(); i++)
    {
        if (!tickPlace(event)) break;
    }
}

bool Scaffold::tickPlace(BaseTickEvent& event)
{
    auto player = event.mActor;

    auto currentY = player->getPos()->y - 2.62f;
    if (!mLockY.mValue) mStartY = currentY;
    if (player->getPos()->y - 2.62f < mStartY) mStartY = player->getPos()->y - 2.62f;
    // If space is held unlock Y
    if (player->getMoveInputComponent()->mIsJumping && !Keyboard::isUsingMoveKeys()) mStartY = currentY;
    float yaw = player->getActorRotationComponent()->mYaw + MathUtils::getRotationKeyOffset() + 90;

    glm::vec3 velocity = player->getStateVectorComponent()->mVelocity;

    float maxExtend = mExtend.mValue;

    if (ItemUtils::getAllPlaceables(mHotbarOnly.mValue) == 0) return false;

    if (mPlacementMode.as<PlacementMode>() == PlacementMode::Flareon)
    {
        yaw = MathUtils::snapYaw(yaw);
    }

    glm::vec3 blockPos = getPlacePos(0.f);


    if (!Keyboard::isUsingMoveKeys())
    {
        maxExtend = 0.f;
    }

    if (!BlockUtils::isAirBlock(blockPos))
    {
        for (float i = 0.f; i < maxExtend; i += 1.f)
        {
            blockPos = getPlacePos(i);
            if (BlockUtils::isAirBlock(blockPos)) break;
        }
    }

    if (!BlockUtils::isValidPlacePos(blockPos)) return false;
    if (!BlockUtils::isAirBlock(blockPos)) return false;
    int side = BlockUtils::getBlockPlaceFace(blockPos);
    mLastSwitchTime = NOW;

    if (mSwitchMode.as<SwitchMode>() == SwitchMode::Full)
    {
        int slot = ItemUtils::getPlaceableItemOnBlock(blockPos, mHotbarOnly.mValue, mSwitchPriority.as<SwitchPriority>() == SwitchPriority::Highest);
        if (slot == -1) return false;
        player->getSupplies()->mSelectedSlot = slot;
    }
    mLastBlock = blockPos;
    mLastFace = side;
    mShouldRotate = true;

    if (mSwing.mValue) player->swing();
    BlockUtils::placeBlock(blockPos, side);

    return true;
}

void Scaffold::onRenderEvent(RenderEvent& event)
{

}

void Scaffold::onPacketOutEvent(PacketOutEvent& event)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    if (event.packet->getId() == PacketID::InventoryTransaction && mFlareonV2Placement.mValue)
    {
        if (const auto it = event.getPacket<InventoryTransactionPacket>(); it->mTransaction->type ==
            ComplexInventoryTransaction::Type::ItemUseTransaction)
        {
            const auto transac = reinterpret_cast<ItemUseInventoryTransaction*>(it->mTransaction.get());
            if (transac->actionType == ItemUseInventoryTransaction::ActionType::Place)
                transac->clickPos = glm::vec3(0, 0, 0);
        }
    }

    if (event.packet->getId() == PacketID::PlayerAuthInput)
    {
        auto paip = event.getPacket<PlayerAuthInputPacket>();

        if (mShouldRotate && mRotateMode.as<RotateMode>() != RotateMode::None)
        {
            glm::vec3 side = BlockUtils::blockFaceOffsets[mLastFace] * 0.5f;
            glm::vec3 target = mLastBlock + side;

            glm::vec2 rotations = MathUtils::getRots(*player->getPos(), target);

            if (mRotateMode.as<RotateMode>() == RotateMode::Normal) {
                rotations.y = player->getActorRotationComponent()->mYaw + MathUtils::getRotationKeyOffset();
                if (rotations.y > 180.f) rotations.y -= 360.f;
                if (rotations.y < -180.f) rotations.y += 360.f;
                rotations.x = fmax(82, rotations.x);
            }

            if (mRotateMode.as<RotateMode>() == RotateMode::Down) rotations.x = 89.9f;

            bool flickRotate = false;

            auto auraMod = gFeatureManager->mModuleManager->getModule<Aura>();

            if (auraMod->mHasTarget) flickRotate = true;

            if (flickRotate) mShouldRotate = false;
            else
            {
                // If the last block placed was more than 500ms ago, then ShouldRotate = false
                if (NOW - mLastSwitchTime > 500) mShouldRotate = false;
            }

            paip->mRot = rotations;
            paip->mYHeadRot = rotations.y;
        }
    }
}

glm::vec3 Scaffold::getRotBasedPos(float extend, float yPos)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    glm::vec2 playerRots = glm::vec2(player->getActorRotationComponent()->mPitch,
                                 player->getActorRotationComponent()->mYaw + MathUtils::getRotationKeyOffset());

    if (mPlacementMode.as<PlacementMode>() == PlacementMode::Flareon)
    {
        playerRots.y = MathUtils::snapYaw(playerRots.y);
    }

    float correctedYaw = (playerRots.y + 90) * ((float)M_PI / 180);
    float inFrontX = cos(correctedYaw) * extend;
    float inFrontZ = sin(correctedYaw) * extend;
    float placeX = player->getPos()->x + inFrontX;
    float placeY = yPos;
    float placeZ = player->getPos()->z + inFrontZ;

    // Floor the values
    return {floor(placeX), floor(placeY), floor(placeZ)};
}

glm::vec3 Scaffold::getPlacePos(float extend)
{
    float yPos = mStartY;
    glm::ivec3 blockSel = {INT_MAX, INT_MAX, INT_MAX};

    blockSel = getRotBasedPos(extend, yPos);

    int side = BlockUtils::getBlockPlaceFace(blockSel);

    if (side == -1)
    {
        // Find da block
        blockSel = BlockUtils::getClosestPlacePos(blockSel, mRange.as<float>());
        if (blockSel.x == INT_MAX) return {FLT_MAX, FLT_MAX, FLT_MAX};
        side = BlockUtils::getBlockPlaceFace(blockSel);

        if (side == -1) return {FLT_MAX, FLT_MAX, FLT_MAX};
    }

    if (blockSel.x == INT_MAX) return {FLT_MAX, FLT_MAX, FLT_MAX};

    return blockSel;
}