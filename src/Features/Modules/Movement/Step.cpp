//
// Created by vastrakai on 8/3/2024.
//

#include "Step.hpp"

#include <Features/Events/BaseTickEvent.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>

void Step::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &Step::onBaseTickEvent>(this);
}

void Step::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &Step::onBaseTickEvent>(this);
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    player->getMaxAutoStepComponent()->mMaxStepHeight = 0.5625f;
}

void Step::onBaseTickEvent(BaseTickEvent& event)
{
    auto player = event.mActor;
    if (!player) return;

    player->getMaxAutoStepComponent()->mMaxStepHeight = mStepHeight.mValue;
}
