//
// Created by alteik on 15/10/2024.
//

#include "ClickTp.hpp"
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/World/Level.hpp>

void ClickTp::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &ClickTp::onBaseTickEvent>(this);
}

void ClickTp::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &ClickTp::onBaseTickEvent>(this);
}

void ClickTp::onBaseTickEvent(BaseTickEvent& event)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    HitResult* hitResult = player->getLevel()->getHitResult();
    if(hitResult->mType == HitType::BLOCK)
    {
        glm::vec3 blockPos = hitResult->mBlockPos;
        glm::vec3 newPos = glm::vec3(blockPos.x, blockPos.y + 1.01 + PLAYER_HEIGHT, blockPos.z);
        player->setPosition(newPos);
    }

    setEnabled(false);
}


