//
// Created by alteik on 15/10/2024.
//

#include "ClickTp.hpp"
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/World/HitResult.hpp>
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

    if (HitResult* hitResult = player->getLevel()->getHitResult(); hitResult->mType == HitType::BLOCK) {
        glm::vec3 blockPos = hitResult->mBlockPos;
        glm::vec3 newPos = {blockPos.x, blockPos.y + 1.01f + PLAYER_HEIGHT, blockPos.z};

        static bool lastRightClick = false;
        if (ImGui::IsMouseDown(1) && !lastRightClick) {
            player->setPosition(newPos);
        }
        lastRightClick = ImGui::IsMouseDown(1);
    }
}