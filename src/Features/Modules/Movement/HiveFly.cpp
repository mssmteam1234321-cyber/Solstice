//
// Created by alteik on 02/09/2024.
//

#include "HiveFly.hpp"
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/MinecraftSim.hpp>
#include <SDK/Minecraft/Network/Packets/PlayerAuthInputPacket.hpp>
#include <Features/Events/PacketOutEvent.hpp>
#include <Features/Events/PacketInEvent.hpp>
#include <SDK/Minecraft/Network/Packets/SetActorMotionPacket.hpp>

bool HiveFly::canFly = false;
int HiveFly::veloTick = 1;
int HiveFly::ticksToStay = 0;
bool HiveFly::shouldStay = false;

void HiveFly::Reset()
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if(!player) return;
    canFly = false;
    veloTick = 1;
    ticksToStay = 0;
    shouldStay = false;
    player->setStatusFlag(ActorFlags::Noai, false);
    ClientInstance::get()->getMinecraftSim()->setSimTimer(20.f);
}

void HiveFly::onEnable()
{
    Reset();
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &HiveFly::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketOutEvent, &HiveFly::onPacketOutEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketInEvent, &HiveFly::onPacketInEvent>(this);
}

void HiveFly::onDisable()
{
    Reset();
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &HiveFly::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketOutEvent, &HiveFly::onPacketOutEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketInEvent, &HiveFly::onPacketInEvent>(this);
}

void HiveFly::onBaseTickEvent(class BaseTickEvent &event) {
    auto player = ClientInstance::get()->getLocalPlayer();
    if(!player) return;

    if(!canFly && shouldStay && ticksToStay)
    {
        ticksToStay--;
        ChatUtils::displayClientMessage("bipass x" +  std::to_string(ticksToStay));
        player->setStatusFlag(ActorFlags::Noai, true);
    }
    else if(!canFly && shouldStay && !ticksToStay)
    {
        Reset();
        ChatUtils::displayClientMessage("successful!");
        HiveFly::setEnabled(false);
    }

    if(mTimer.mValue > 0 && canFly)
    {
        ClientInstance::get()->getMinecraftSim()->setSimTimer(mTimer.mValue);
    }

    if(canFly && !shouldStay) {
        glm::vec3 motion = glm::vec3(0, 0, 0);

        if (Keyboard::isUsingMoveKeys(true)) {
            glm::vec2 calc = MathUtils::getMotion(player->getActorRotationComponent()->mYaw, mSpeed.mValue / 10);
            motion.x = calc.x;
            motion.z = calc.y;
            motion.y -= 0.1;
        }

        player->getStateVectorComponent()->mVelocity = motion;
    }
}

void HiveFly::onPacketOutEvent(class PacketOutEvent &event) {
    auto player = ClientInstance::get()->getLocalPlayer();
    if(!player) return;

    if(canFly && event.mPacket->getId() == PacketID::PlayerAuthInput)
    {
        auto paip = event.getPacket<PlayerAuthInputPacket>();

        switch (veloTick)
        {
            case 1:
                paip->mPosDelta.y = 0.300000;
                ChatUtils::displayClientMessage("Gaming x1");
                veloTick++;
                break;
            case 2:
                paip->mPosDelta.y = 0.215600;
                ChatUtils::displayClientMessage("Gaming x2");
                veloTick++;
                break;
            case 3:
                paip->mPosDelta.y = 0.132888;
                ChatUtils::displayClientMessage("Gaming x3");
                veloTick++;
                break;
            case 4:
                paip->mPosDelta.y = 0.051830;
                ChatUtils::displayClientMessage("Gaming x4");
                veloTick++;
                shouldStay = true;
                ticksToStay = 2;
                break;
            case 5:
                paip->mPosDelta.y = -0.027606;
                ChatUtils::displayClientMessage("Gaming x5");
                veloTick = 1;
                ChatUtils::displayClientMessage("Reset!");
                canFly = false;
                break;
            default:
                break;
        }
    }
}

void HiveFly::onPacketInEvent(class PacketInEvent &event) {
    auto player = ClientInstance::get()->getLocalPlayer();
    if(!player) return;

    if(event.mPacket->getId() == PacketID::SetActorMotion)
    {
        auto sam = event.getPacket<SetActorMotionPacket>();
        if (sam->mRuntimeID == player->getRuntimeID()) {
            canFly = true;
            ChatUtils::displayClientMessage("Damage taken!");
        }
    }
}