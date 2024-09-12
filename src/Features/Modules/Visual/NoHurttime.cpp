//
// Created by alteik on 02/09/2024.
//

#include "NoHurttime.hpp"
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>

void NoHurtcam::onEnable()
{
        gFeatureManager->mDispatcher->listen<BaseTickEvent, &NoHurtcam::onBaseTickEvent>(this);
        gFeatureManager->mDispatcher->listen<RenderEvent, &NoHurtcam::onRenderEvent>(this);
}

void NoHurtcam::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &NoHurtcam::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->deafen<RenderEvent, &NoHurtcam::onRenderEvent>(this);
}
// TODO: Replace this with a camera event from a hook to intercept the camera rotation and cancel it

void NoHurtcam::onBaseTickEvent(class BaseTickEvent &event) {
    auto player = ClientInstance::get()->getLocalPlayer();
    if(!player) return;

    player->getMobHurtTimeComponent()->mHurtTime = 0;
}

void NoHurtcam::onRenderEvent(class RenderEvent &event) {
    auto player = ClientInstance::get()->getLocalPlayer();
    if(!player) return;

    player->getMobHurtTimeComponent()->mHurtTime = 0;
}