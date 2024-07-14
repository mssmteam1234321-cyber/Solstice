//
// Created by vastrakai on 7/1/2024.
//

#include "Interface.hpp"

#include <Features/Events/ActorRenderEvent.hpp>
#include <Features/Events/BaseTickEvent.hpp>
#include <Features/Events/ModuleStateChangeEvent.hpp>
#include <Features/Events/PacketInEvent.hpp>
#include <Features/Events/PacketOutEvent.hpp>
#include <Features/Events/RenderEvent.hpp>
#include <Hook/Hooks/RenderHooks/ActorRenderDispatcherHook.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>

#include <SDK/Minecraft/Options.hpp>
#include <SDK/Minecraft/Network/Packets/Packet.hpp>
#include <SDK/Minecraft/Network/Packets/PlayerAuthInputPacket.hpp>

float pYaw;
float pOldYaw;

float pHeadYaw;
float pOldHeadYaw;

float pPitch;
float pOldPitch;

float pBodyYaw;
float pOldBodyYaw;

float pLerpedYaw;
float pLerpedHeadYaw;
float pLerpedPitch;
float pLerpedBodyYaw;

void Interface::onEnable()
{

}

void Interface::onDisable()
{

}

void Interface::onModuleStateChange(ModuleStateChangeEvent& event)
{
    if (event.mModule == this)
    {
        event.setCancelled(true);
    }
}

void Interface::onRenderEvent(RenderEvent& event)
{
    static constexpr float LERP_SPEED = 20.f;
    float deltaTime = ImGui::GetIO().DeltaTime;

    float yaw = MathUtils::wrap(pLerpedYaw, pYaw - 180, pYaw + 180);
    float headYaw = MathUtils::wrap(pLerpedHeadYaw, pHeadYaw - 180, pHeadYaw + 180);
    float pitch = pLerpedPitch;
    float bodyYaw = MathUtils::wrap(pLerpedBodyYaw, pBodyYaw - 180, pBodyYaw + 180);

    float preLerpedYaw = MathUtils::lerp(yaw, pYaw, deltaTime * LERP_SPEED);
    float preLerpedHeadYaw = MathUtils::lerp(headYaw, pHeadYaw, deltaTime * LERP_SPEED);
    float preLerpedPitch = MathUtils::lerp(pitch, pPitch, deltaTime * LERP_SPEED);
    float preLerpedBodyYaw = MathUtils::lerp(bodyYaw, pBodyYaw, deltaTime * LERP_SPEED);

    pLerpedYaw = MathUtils::wrap(pLerpedYaw, preLerpedYaw - 180, preLerpedYaw + 180);
    pLerpedHeadYaw = MathUtils::wrap(pLerpedHeadYaw, preLerpedHeadYaw - 180, preLerpedHeadYaw + 180);
    pLerpedBodyYaw = MathUtils::wrap(pLerpedBodyYaw, preLerpedBodyYaw - 180, preLerpedBodyYaw + 180);

    pLerpedYaw = MathUtils::lerp(preLerpedYaw, pLerpedYaw, deltaTime * LERP_SPEED);
    pLerpedHeadYaw = MathUtils::lerp(preLerpedHeadYaw, pLerpedHeadYaw, deltaTime * LERP_SPEED);
    pLerpedPitch = MathUtils::lerp(preLerpedPitch, pLerpedPitch, deltaTime * LERP_SPEED);
    pLerpedBodyYaw = MathUtils::lerp(preLerpedBodyYaw, pLerpedBodyYaw, deltaTime * LERP_SPEED);

}

void Interface::onActorRenderEvent(ActorRenderEvent& event)
{
    bool firstPerson = ClientInstance::get()->getOptions()->game_thirdperson->value == 0;
    if (firstPerson) return;
    const auto actorRotations = event.mEntity->getActorRotationComponent();
    const auto headRotations = event.mEntity->getActorHeadRotationComponent();
    const auto bodyRotations = event.mEntity->getMobBodyRotationComponent();
    if (!actorRotations || !headRotations || !bodyRotations) return;

    float realOldPitch = actorRotations->mOldPitch;
    float realPitch = actorRotations->mPitch;
    float realHeadRot = headRotations->headRot;
    float realOldHeadRot = headRotations->oldHeadRot;
    float realBodyYaw = bodyRotations->yBodyRot;
    float realOldBodyYaw = bodyRotations->yOldBodyRot;

    actorRotations->mOldPitch = pLerpedPitch;
    actorRotations->mPitch = pLerpedPitch;
    headRotations->headRot = pLerpedHeadYaw;
    headRotations->oldHeadRot = pLerpedHeadYaw;
    bodyRotations->yBodyRot = pLerpedBodyYaw;
    bodyRotations->yOldBodyRot = pLerpedBodyYaw;

    auto original = event.mDetour->getOriginal<decltype(&ActorRenderDispatcherHook::render)>();
    original(event._this, event.mEntityRenderContext, event.mEntity, event.mCameraTargetPos, event.mPos, event.mRot, event.mIgnoreLighting);
    event.cancel();

    actorRotations->mOldPitch = realOldPitch;
    actorRotations->mPitch = realPitch;
    headRotations->headRot = realHeadRot;
    headRotations->oldHeadRot = realOldHeadRot;
    bodyRotations->yBodyRot = realBodyYaw;
    bodyRotations->yOldBodyRot = realOldBodyYaw;
}


void Interface::onBaseTickEvent(BaseTickEvent& event)
{
    auto player = ClientInstance::get()->getLocalPlayer();

    BodyYaw::updateRenderAngles(player, pYaw);
    pOldBodyYaw = pBodyYaw;
    pBodyYaw = BodyYaw::bodyYaw;
}

void Interface::onPacketOutEvent(PacketOutEvent& event)
{
    if (event.packet->getId() == PacketID::PlayerAuthInput)
    {
        auto paip = event.getPacket<PlayerAuthInputPacket>();

        pOldYaw = pYaw;
        pOldPitch = pPitch;
        pOldBodyYaw = pBodyYaw;
        pYaw = paip->mRot.y;
        pPitch = paip->mRot.x;
        pHeadYaw = paip->mYHeadRot;
    }
}
