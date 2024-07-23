//
// Created by vastrakai on 7/22/2024.
//

#include "Freecam.hpp"

#include <Features/Events/ActorRenderEvent.hpp>
#include <SDK/Minecraft/Network/MinecraftPackets.hpp>
#include <SDK/Minecraft/Network/Packets/CameraInstructionPacket.hpp>
#include <Features/Events/BaseTickEvent.hpp>
#include <Features/Events/LookInputEvent.hpp>
#include <Features/Events/PacketInEvent.hpp>
#include <Features/Events/PacketOutEvent.hpp>
#include <Hook/Hooks/NetworkHooks/PacketReceiveHook.hpp>
#include <Hook/Hooks/RenderHooks/ActorRenderDispatcherHook.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Options.hpp>
#include <SDK/Minecraft/Network/Packets/MovePlayerPacket.hpp>
#include <SDK/Minecraft/Network/Packets/TextPacket.hpp>

void Freecam::onEnable()
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    player->setFlag<RenderCameraFlag>(true);
    player->setFlag<CameraRenderPlayerModel>(true);
    player->setFlag<CameraRenderFirstPersonObjects>(false);

    gFeatureManager->mDispatcher->listen<BaseTickEvent, &Freecam::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->listen<LookInputEvent, &Freecam::onLookInputEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketInEvent, &Freecam::onPacketInEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketOutEvent, &Freecam::onPacketOutEvent>(this);
    gFeatureManager->mDispatcher->listen<ActorRenderEvent, &Freecam::onActorRenderEvent, nes::event_priority::VERY_FIRST>(this);



    mLastRot = *player->getActorRotationComponent();
    mLastRot.mOldYaw = mLastRot.mYaw;
    mLastRot.mOldPitch = mLastRot.mPitch;

    mLastHeadRot = *player->getActorHeadRotationComponent();
    mLastHeadRot.oldHeadRot = mLastHeadRot.headRot;

    mLastBodyRot = *player->getMobBodyRotationComponent();
    mLastBodyRot.yOldBodyRot = mLastBodyRot.yBodyRot;

    mAABBMin = player->getAABBShapeComponent()->mMin;
    mAABBMax = player->getAABBShapeComponent()->mMax;
    mSvPos = player->getStateVectorComponent()->mPos;
    mSvPosOld = player->getStateVectorComponent()->mPos;
    mOldPos = player->getRenderPositionComponent()->mPosition;
    player->getWalkAnimationComponent()->mWalkAnimSpeed = 0.0f;
}

void Freecam::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &Freecam::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->deafen<LookInputEvent, &Freecam::onLookInputEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketInEvent, &Freecam::onPacketInEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketOutEvent, &Freecam::onPacketOutEvent>(this);
    gFeatureManager->mDispatcher->deafen<ActorRenderEvent, &Freecam::onActorRenderEvent>(this);

    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    player->setFlag<RenderCameraFlag>(false);
    player->setFlag<CameraRenderPlayerModel>(false);
    player->setFlag<CameraRenderFirstPersonObjects>(false);

    player->getAABBShapeComponent()->mMin = mAABBMin;
    player->getAABBShapeComponent()->mMax = mAABBMax;
    player->getStateVectorComponent()->mPos = mSvPos;
    player->getStateVectorComponent()->mPosOld = mSvPosOld;
    player->getWalkAnimationComponent()->mWalkAnimSpeed = 1.0f;
}

void Freecam::onPacketInEvent(PacketInEvent& event)
{
    if (!mDisableOnLagback.mValue) return;

    if (event.mPacket->getId() == PacketID::MovePlayer)
    {
        spdlog::info("MovePlayerPacket");
        auto player = ClientInstance::get()->getLocalPlayer();
        if (!player) return;

        auto mpp = event.getPacket<MovePlayerPacket>();
        if (mpp->mPlayerID == player->getRuntimeID())
        {
            NotifyUtils::notify("Lagback detected!", 5.f, Notification::Type::Warning);
            setEnabled(false);
        }
    }
}

void Freecam::onPacketOutEvent(PacketOutEvent& event)
{
    if (event.mPacket->getId() == PacketID::PlayerAuthInput || event.mPacket->getId() == PacketID::MovePlayer)
        event.mCancelled = true;

}

void Freecam::onBaseTickEvent(BaseTickEvent& event)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    player->setFlag<RenderCameraFlag>(true);
    player->setFlag<CameraRenderPlayerModel>(true);
    player->setFlag<CameraRenderFirstPersonObjects>(false);

    glm::vec3 motion = glm::vec3(0, 0, 0);

    if (Keyboard::isUsingMoveKeys(true))
    {
        glm::vec2 calc = MathUtils::getMotion(player->getActorRotationComponent()->mYaw, mSpeed.mValue / 10);
        motion.x = calc.x;
        motion.z = calc.y;

        bool isJumping = player->getMoveInputComponent()->mIsJumping;
        bool isSneaking = player->getMoveInputComponent()->mIsSneakDown;

        if (isJumping)
            motion.y += mSpeed.mValue / 10;
        else if (isSneaking)
            motion.y -= mSpeed.mValue / 10;
    }

    player->getStateVectorComponent()->mVelocity = motion;
}

void Freecam::onActorRenderEvent(ActorRenderEvent& event)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    auto oldRots = *player->getActorRotationComponent();
    auto oldHeadRots = *player->getActorHeadRotationComponent();
    auto oldBodyRots = *player->getMobBodyRotationComponent();

    *player->getActorRotationComponent() = mLastRot;
    *player->getActorHeadRotationComponent() = mLastHeadRot;
    *player->getMobBodyRotationComponent() = mLastBodyRot;

    auto original = event.mDetour->getOriginal<decltype(&ActorRenderDispatcherHook::render)>();
    auto newPos = *event.mPos - *event.mCameraTargetPos - *event.mPos + mOldPos;
    original(event._this, event.mEntityRenderContext, event.mEntity, event.mCameraTargetPos, &newPos, event.mRot, event.mIgnoreLighting);
    event.cancel();

    *player->getActorRotationComponent() = oldRots;
    *player->getActorHeadRotationComponent() = oldHeadRots;
    *player->getMobBodyRotationComponent() = oldBodyRots;
}

void Freecam::onLookInputEvent(LookInputEvent& event)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    ClientInstance::get()->getOptions()->game_thirdperson->value = 0;
}
