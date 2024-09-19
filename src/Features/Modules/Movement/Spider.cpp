//
// Created by ssi on 8/17/2024.
//

#include "Spider.hpp"
#include <Features/Events/BaseTickEvent.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>

void Spider::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &Spider::onBaseTickEvent>(this);
}

void Spider::onBaseTickEvent(BaseTickEvent& event) const {
    const auto player = event.mActor;
    if (!player) return;
    if (mOnGroundOnly.mValue && !player->isOnGround()) return;

    if (mMode.mValue == Mode::Clip && player->isCollidingHorizontal() && player->getMoveInputComponent()->mForward) {
        const auto state = player->getStateVectorComponent();
        const auto aabbShape = player->getAABBShapeComponent();

        state->mVelocity.y = 0.f;

        aabbShape->mMin.y += mSpeed.mValue;
        aabbShape->mMax.y += mSpeed.mValue;
    }
}

void Spider::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &Spider::onBaseTickEvent>(this);
}