//
// Created by vastrakai on 7/2/2024.
//

#include "NoSlowDown.hpp"

#include <Features/FeatureManager.hpp>
#include <Features/Events/ItemSlowdownEvent.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>

void NoSlowDown::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &NoSlowDown::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->listen<ItemSlowdownEvent, &NoSlowDown::onItemSlowdownEvent>(this);
}

void NoSlowDown::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &NoSlowDown::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->deafen<ItemSlowdownEvent, &NoSlowDown::onItemSlowdownEvent>(this);
}

void NoSlowDown::onBaseTickEvent(BaseTickEvent& event)
{
    auto player = ClientInstance::get()->getLocalPlayer();

    auto slowdownComponent = player->getBlockMovementSlowdownMultiplierComponent();

    slowdownComponent->mBlockMovementSlowdownMultiplier = glm::vec3(0.f, 0.f, 0.f);
}

void NoSlowDown::onItemSlowdownEvent(ItemSlowdownEvent& event)
{
    event.cancel();
}