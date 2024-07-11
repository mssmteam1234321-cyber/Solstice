//
// Created by vastrakai on 7/10/2024.
//

#include "Speed.hpp"

#include <Features/FeatureManager.hpp>
#include <Features/Events/BaseTickEvent.hpp>
#include <Features/Events/PacketOutEvent.hpp>
#include <Features/Events/RunUpdateCycleEvent.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/MinecraftSim.hpp>
#include <SDK/Minecraft/Network/Packets/PlayerAuthInputPacket.hpp>
#include <Utils/Keyboard.hpp>
#include <Utils/MiscUtils/EasingUtil.hpp>
#include <Utils/MiscUtils/MathUtils.hpp>


void Speed::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &Speed::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketOutEvent, &Speed::onPacketOutEvent>(this);
    gFeatureManager->mDispatcher->listen<RunUpdateCycleEvent, &Speed::onRunUpdateCycleEvent>(this);


}

void Speed::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &Speed::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketOutEvent, &Speed::onPacketOutEvent>(this);
    gFeatureManager->mDispatcher->deafen<RunUpdateCycleEvent, &Speed::onRunUpdateCycleEvent>(this);
    ClientInstance::get()->getMinecraftSim()->setSimTimer(20.f);
}

void Speed::onRunUpdateCycleEvent(RunUpdateCycleEvent& event)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    if (mMode.as<Mode>() != Mode::Friction) {
        Sleep(101);
    } else if (mApplyNetskip.mValue) {
        Sleep(101);
    }

    spdlog::info("RunUpdateCycleEvent");
}

void Speed::onBaseTickEvent(BaseTickEvent& event)
{
    auto player = event.mActor;
    if (!player) return;

    if (mMode.as<Mode>() == Mode::Friction)
    {
        tickFriction(player);
    }
}

void Speed::onPacketOutEvent(PacketOutEvent& event)
{
    if (event.packet->getId() == PacketID::PlayerAuthInput)
    {
        auto paip = event.getPacket<PlayerAuthInputPacket>();
        auto player = ClientInstance::get()->getLocalPlayer();
        if (Keyboard::isUsingMoveKeys()) paip->mInputData |= AuthInputAction::JUMPING | AuthInputAction::WANT_UP | AuthInputAction::JUMP_DOWN;
        if (!player->isOnGround() && player->wasOnGround() && Keyboard::isUsingMoveKeys()) {
            paip->mInputData |= AuthInputAction::START_JUMPING;
        }
    }
}

void Speed::tickFriction(Actor* player)
{
    if (mTimerBoost.mValue) ClientInstance::get()->getMinecraftSim()->setSimTimer(mTimerSpeed.as<float>());
    else ClientInstance::get()->getMinecraftSim()->setSimTimer(20.f);

    static auto friction = mFriction.as<float>();

    if (player->isOnGround()) friction = 1.f;
    else friction *= mFriction.as<float>();

    glm::vec2 motion = MathUtils::getMotion(player->getActorRotationComponent()->mYaw, (mSpeed.as<float>() / 10) * friction);
    auto stateVector = player->getStateVectorComponent();
    stateVector->mVelocity = {motion.x, stateVector->mVelocity.y, motion.y};

    bool usingMoveKeys = Keyboard::isUsingMoveKeys();
    if (usingMoveKeys && mJumpType.as<JumpType>() == JumpType::Vanilla) {
        if (player->isOnGround()) player->jumpFromGround();
    } else if (usingMoveKeys && mJumpType.as<JumpType>() == JumpType::Velocity) {
        if (player->isOnGround()) {
            player->getStateVectorComponent()->mVelocity.y = mJumpHeight.as<float>();
        }
    }
};