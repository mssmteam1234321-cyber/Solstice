//
// Created by dark on 9/19/2024.
//

#include "TargetStrafe.hpp"

#include <Features/Modules/Combat/Aura.hpp>
#include <Features/Modules/Movement/Speed.hpp>

#include <Features/FeatureManager.hpp>
#include <Features/Events/BaseTickEvent.hpp>
#include <Features/Events/RenderEvent.hpp>
#include <Features/Events/PacketOutEvent.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/Network/Packets/PlayerAuthInputPacket.hpp>

void TargetStrafe::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &TargetStrafe::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->listen<RenderEvent, &TargetStrafe::onRenderEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketOutEvent, &TargetStrafe::onPacketOutEvent, nes::event_priority::VERY_LAST>(this);
}

void TargetStrafe::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &TargetStrafe::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->deafen<RenderEvent, &TargetStrafe::onRenderEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketOutEvent, &TargetStrafe::onPacketOutEvent>(this);

    mShouldStrafe = false;
}

void TargetStrafe::onBaseTickEvent(BaseTickEvent& event)
{
    auto player = event.mActor;
    glm::vec3 playerPos = *player->getPos();
    auto moveInputComponent = player->getMoveInputComponent();

    if (!Aura::sHasTarget || !Aura::sTarget || !Aura::sTarget->getActorTypeComponent())
    {
        mShouldStrafe = false;
        return;
    }

    if (mJumpOnly.mValue && !mIsJumping) {
        mShouldStrafe = false;
        return;
    }

    mCurrentTarget = Aura::sTarget;
    glm::vec3 targetPos = *mCurrentTarget->getPos();
    float dist = glm::distance(glm::vec2(playerPos.x, playerPos.z), glm::vec2(targetPos.x, targetPos.z));
    bool foward = mDistance.mValue < dist;
    bool back = dist < mMinDistance.mValue;

    if (mWallCheck.mValue && player->isCollidingHorizontal()) {
        mMoveRight = !mMoveRight;
    }
    else {
        if (moveInputComponent->mLeft) mMoveRight = false;
        else if (moveInputComponent->mRight) mMoveRight = true;
    }

    mYaw = MathUtils::getRots(playerPos, mCurrentTarget->getAABB()).y;

    handleKeyInput(foward, !mMoveRight, back, mMoveRight);
    mShouldStrafe = true;
}

void TargetStrafe::onRenderEvent(RenderEvent& event) 
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    if (mShouldStrafe)
    {
        if (!mCurrentTarget->getActorTypeComponent()) return;
        auto rotationComponent = player->getActorRotationComponent();
        glm::vec3 renderPosition = player->getRenderPositionComponent()->mPosition;
        rotationComponent->mYaw = mYaw;
        rotationComponent->mOldYaw = mYaw;
    }
}

void TargetStrafe::onPacketOutEvent(PacketOutEvent& event)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    if (event.mPacket->getId() == PacketID::PlayerAuthInput)
    {
        auto pkt = event.getPacket<PlayerAuthInputPacket>();
        if (pkt->hasInputData(AuthInputAction::JUMPING)) mIsJumping = true;
        else mIsJumping = false;

        if (mShouldStrafe) {
            if (mAlwaysSprint.mValue) {
                pkt->mInputData |= AuthInputAction::SPRINT_DOWN | AuthInputAction::SPRINTING | AuthInputAction::START_SPRINTING;
                pkt->mInputData &= ~AuthInputAction::STOP_SPRINTING;

                if (mMoveRight) pkt->mMove = { 0.45f, 0.45f };
                else pkt->mMove = { 0.45f, -0.45f };                
            }
        }
    }
}

void TargetStrafe::handleKeyInput(bool pressingW, bool pressingA, bool pressingS, bool pressingD)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    auto moveInputComponent = player->getMoveInputComponent();

    moveInputComponent->mForward = pressingW;
    moveInputComponent->mLeft = pressingA;
    moveInputComponent->mBackward = pressingS;
    moveInputComponent->mRight = pressingD;
}