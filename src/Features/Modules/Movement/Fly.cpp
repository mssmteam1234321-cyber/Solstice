//
// Created by vastrakai on 6/30/2024.
//

#include "Fly.hpp"

#include <Features/FeatureManager.hpp>
#include <Features/Events/PacketOutEvent.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/Actor/Components/StateVectorComponent.hpp>
#include <SDK/Minecraft/Actor/Components/ActorRotationComponent.hpp>
#include <SDK/Minecraft/Network/Packets/PlayerAuthInputPacket.hpp>
#include <Utils/Keyboard.hpp>
#include <Utils/MiscUtils/MathUtils.hpp>
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

class PlayerAuthInputPacket;

void Fly::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &Fly::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketOutEvent, &Fly::onPacketOutEvent>(this);
}

void Fly::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &Fly::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketOutEvent, &Fly::onPacketOutEvent>(this);
}

void Fly::onBaseTickEvent(BaseTickEvent& event) const
{
    if (Mode.mValue == Motion)
    {
        auto player = ClientInstance::get()->getLocalPlayer();
        if (player == nullptr)
            return;

        glm::vec3 motion = glm::vec3(0, 0, 0);

        if (Keyboard::isUsingMoveKeys(true))
        {
            glm::vec2 calc = MathUtils::getMotion(player->getActorRotationComponent()->mYaw, Speed.mValue / 10);
            motion.x = calc.x;
            motion.z = calc.y;

            bool isJumping = player->getMoveInputComponent()->mIsJumping;
            bool isSneaking = player->getMoveInputComponent()->mIsSneakDown;

            if (isJumping)
                motion.y += Speed.mValue / 10;
            else if (isSneaking)
                motion.y -= Speed.mValue / 10;
        }

        player->getStateVectorComponent()->mVelocity = motion;
    }
}

void Fly::onPacketOutEvent(PacketOutEvent& event) const
{

    if (event.packet->getId() == PacketID::PlayerAuthInput)
    {
        auto player = ClientInstance::get()->getLocalPlayer();
        if (player == nullptr)
            return;

        auto packet = reinterpret_cast<PlayerAuthInputPacket*>(event.packet);
        if (Mode.mValue == Motion && ApplyGlideFlags.mValue)
        {
            packet->mInputData |= AuthInputAction::START_GLIDING;
            packet->mInputData &= ~AuthInputAction::STOP_GLIDING;
        }
    }
}
