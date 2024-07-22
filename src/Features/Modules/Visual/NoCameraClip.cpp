//
// Created by vastrakai on 7/22/2024.
//

#include "NoCameraClip.hpp"

#include <Features/Events/LookInputEvent.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>

void NoCameraClip::onEnable()
{
    gFeatureManager->mDispatcher->listen<LookInputEvent, &NoCameraClip::onLookInputEvent>(this);
}


void NoCameraClip::onDisable()
{
    gFeatureManager->mDispatcher->deafen<LookInputEvent, &NoCameraClip::onLookInputEvent>(this);
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;
    if (!mFirstPersonCamera || !mThirdPersonCamera || !mThirdPersonFrontCamera) return;
    mFirstPersonCamera->mFov.z = 0.025f;
    mThirdPersonCamera->mFov.z = 0.025f;
    mThirdPersonFrontCamera->mFov.z = 0.025f;
}

void NoCameraClip::onLookInputEvent(LookInputEvent& event)
{
    mFirstPersonCamera = event.mFirstPersonCamera;
    mThirdPersonCamera = event.mThirdPersonCamera;
    mThirdPersonFrontCamera = event.mThirdPersonFrontCamera;
    event.mFirstPersonCamera->mFov.z = 2500.0f;
    event.mThirdPersonCamera->mFov.z = 2500.0f;
    event.mThirdPersonFrontCamera->mFov.z = 2500.0f;

}
