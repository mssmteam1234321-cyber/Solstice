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
#include <SDK/Minecraft/Inventory/PlayerInventory.hpp>
#include <SDK/Minecraft/Network/Packets/MovePlayerPacket.hpp>
#include <SDK/Minecraft/Network/Packets/PlayerAuthInputPacket.hpp>
#include <SDK/Minecraft/Network/Packets/TextPacket.hpp>
#include <SDK/Minecraft/World/HitResult.hpp>
#include <SDK/Minecraft/World/Level.hpp>

void Freecam::onEnable()
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    player->setFlag<RenderCameraFlag>(true);
    player->setFlag<CameraRenderPlayerModel>(true);
    player->setFlag<RedirectCameraInput>(true);

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
    if (mMode.mValue != Mode::Detached)
        player->getWalkAnimationComponent()->mWalkAnimSpeed = 0.0f;

    mOrigin = mSvPos;
    mOldOrigin = mSvPosOld;
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
    player->setFlag<RedirectCameraInput>(false);

    if (mMode.mValue == Mode::Normal)
    {
        player->getAABBShapeComponent()->mMin = mAABBMin;
        player->getAABBShapeComponent()->mMax = mAABBMax;
        player->getStateVectorComponent()->mPos = mSvPos;
        player->getStateVectorComponent()->mPosOld = mSvPosOld;
    }
    player->getWalkAnimationComponent()->mWalkAnimSpeed = 1.0f;
    player->getMoveInputComponent()->reset( false);
}

void Freecam::onPacketInEvent(PacketInEvent& event)
{

    if (event.mPacket->getId() == PacketID::MovePlayer)
    {
        spdlog::info("MovePlayerPacket");
        auto player = ClientInstance::get()->getLocalPlayer();
        if (!player) return;

        auto mpp = event.getPacket<MovePlayerPacket>();
        if (mpp->mPlayerID == player->getRuntimeID())
        {
            if (mDisableOnLagback.mValue)
            {
                NotifyUtils::notify("Lagback detected!", 5.f, Notification::Type::Warning);
                setEnabled(false);
                return;
            }
            glm::vec2 pktRot = mpp->mRot;
            float headRot = mpp->mYHeadRot;
            mLastRot.mYaw = pktRot.x;
            mLastRot.mPitch = pktRot.y;
            mLastHeadRot.headRot = headRot;
            mLastBodyRot.yBodyRot = pktRot.x;

            mLastRot.mOldYaw = pktRot.x;
            mLastRot.mOldPitch = pktRot.y;
            mLastHeadRot.oldHeadRot = headRot;
            mLastBodyRot.yOldBodyRot = pktRot.x;


            player->setPosition(mpp->mPos);
            mOrigin = mpp->mPos;
            mOldOrigin = mpp->mPos;
            spdlog::debug("[Freecam] Listening to lagback, setting position to {}, {}, {}", mpp->mPos.x, mpp->mPos.y, mpp->mPos.z);
        }
    }
}

void Freecam::onPacketOutEvent(PacketOutEvent& event)
{
    if ((event.mPacket->getId() == PacketID::PlayerAuthInput || event.mPacket->getId() == PacketID::MovePlayer) && mMode.mValue != Mode::Detached)
        event.mCancelled = true;

    if ((event.mPacket->getId() == PacketID::PlayerAuthInput) && mMode.mValue == Mode::Detached)
    {
        auto player = ClientInstance::get()->getLocalPlayer();
        if (!player) return;
        auto paip = event.getPacket<PlayerAuthInputPacket>();

        paip->mRot = { mLastRot.mYaw, mLastRot.mPitch };
        paip->mYHeadRot = mLastHeadRot.headRot;

        // Clamp the rot to -180 to 180 and -90 to 90 respectively
        paip->mRot.x = MathUtils::wrap(paip->mRot.x, -180, 180);
        paip->mRot.y = MathUtils::clamp(paip->mRot.y, -90, 90);
        paip->mYHeadRot = MathUtils::wrap(paip->mYHeadRot, -180, 180);

    }

}


void Freecam::onBaseTickEvent(BaseTickEvent& event)
{
    auto player = event.mActor;
    if (!player) return;

    player->getSupplies()->mInHandSlot = -1;

    player->setFlag<RenderCameraFlag>(true);
    player->setFlag<CameraRenderPlayerModel>(true);
    //player->setFlag<CameraRenderFirstPersonObjects>(false);

    glm::vec3 motion = glm::vec3(0, 0, 0);

    if (Keyboard::isUsingMoveKeys(true))
    {
        glm::vec2 rots = mRotRads;
        rots = glm::vec2(rots.y, rots.x); // Correct rotation (pitch, yaw)
        // convert the rots to degrees (-180 to 180 for yaw, -90 to 90 for pitch)
        rots = glm::degrees(rots);
        // Invert the yaw
        rots.y = -rots.y + 180;
        // Wrap
        rots.y = MathUtils::wrap(rots.y, -180, 180);

        glm::vec2 calc = MathUtils::getMotion(rots.y, mSpeed.mValue / 10, false);
        motion.x = calc.x;
        motion.z = calc.y;

        bool isJumping = Keyboard::mPressedKeys[VK_SPACE];
        bool isSneaking = Keyboard::mPressedKeys[VK_SHIFT];

        if (isJumping)
            motion.y += mSpeed.mValue / 10;
        else if (isSneaking)
            motion.y -= mSpeed.mValue / 10;
    }

    if (mMode.mValue == Mode::Normal) player->getStateVectorComponent()->mVelocity = motion;
    else if (mMode.mValue == Mode::Detached)
    {
        mOldOrigin = mOrigin;
        mOrigin += motion;
    }
}

void Freecam::onActorRenderEvent(ActorRenderEvent& event)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    if (event.mEntity != player) return;
    if (*event.mPos == glm::vec3(0.f, 0.f, 0.f) && *event.mRot == glm::vec2(0.f, 0.f)) event.cancel(); // Prevents the hand from rendering

    auto oldRots = *player->getActorRotationComponent();
    auto oldHeadRots = *player->getActorHeadRotationComponent();
    auto oldBodyRots = *player->getMobBodyRotationComponent();

    *player->getActorRotationComponent() = mLastRot;
    *player->getActorHeadRotationComponent() = mLastHeadRot;
    *player->getMobBodyRotationComponent() = mLastBodyRot;
    if (mMode.mValue == Mode::Detached)
    {
        player->getMoveInputComponent()->reset(true, false);
        // TODO: Prevent the raycast from updating while in detached mode
    }

    auto original = event.mDetour->getOriginal<decltype(&ActorRenderDispatcherHook::render)>();
    auto newPos = *event.mPos - *event.mCameraTargetPos - *event.mPos + mOldPos;
    if (mMode.mValue == Mode::Detached) newPos = *event.mPos - *event.mCameraTargetPos - *event.mPos + player->getRenderPositionComponent()->mPosition;
    original(event._this, event.mEntityRenderContext, event.mEntity, event.mCameraTargetPos, &newPos, event.mRot, event.mIgnoreLighting);
    event.cancel();

    if (mMode.mValue != Mode::Detached)
    {
        *player->getActorRotationComponent() = oldRots;
        *player->getActorHeadRotationComponent() = oldHeadRots;
        *player->getMobBodyRotationComponent() = oldBodyRots;
    }
}

void Freecam::onLookInputEvent(LookInputEvent& event)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    ClientInstance::get()->getOptions()->game_thirdperson->value = 0;

    if (mMode.mValue == Mode::Detached)
    {
        event.mFirstPersonCamera->mOrigin = getLerpedOrigin();
    }
    mRotRads = event.mCameraDirectLookComponent->mRotRads;
}
