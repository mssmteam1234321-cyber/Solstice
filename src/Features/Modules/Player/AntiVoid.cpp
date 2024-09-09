//
// Created by vastrakai on 9/8/2024.
//

#include "AntiVoid.hpp"

#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>

void AntiVoid::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &AntiVoid::onBaseTickEvent>(this);
    auto player = ClientInstance::get()->getLocalPlayer();

    if (!player) return;

}

void AntiVoid::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &AntiVoid::onBaseTickEvent>(this);
}

void AntiVoid::onBaseTickEvent(BaseTickEvent& event)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    if (player->isOnGround())
    {
        mOnGroundPositions.push_back(*player->getPos());
        mTeleported = false;

        // if we have more than 40 positions, remove the first one
        if (mOnGroundPositions.size() > 40)
        {
            mOnGroundPositions.erase(mOnGroundPositions.begin());
        }
        return;
    }

    bool hasTeleported = mTeleported && mTpOnce.mValue;

    if (player->getFallDistance() > mFallDistance.mValue && (!hasTeleported || !mTpOnce.mValue))
    {
        glm::vec3 bestPos = glm::ivec3(0, 0, 0);
        bool found = false;
        auto inverted = mOnGroundPositions;
        std::reverse(inverted.begin(), inverted.end());
        for (auto& pos : inverted)
        {
            glm::ivec3 blockPos = glm::ivec3(pos + glm::vec3(0, -1, 0) - PLAYER_HEIGHT_VEC);
            if (BlockUtils::isGoodBlock(blockPos))
            {
                bestPos = blockPos;
                found = true;
                break;
            }
        }

        if (!found)
        {
            NotifyUtils::notify("Could not find a safe position to teleport to!", 5.0f, Notification::Type::Error);
            return;
        }

        player->setPosition(bestPos + glm::vec3(0, 2 + PLAYER_HEIGHT, 0));
        player->setFallDistance(0.0f);
        NotifyUtils::notify("Teleported!", 5.0f, Notification::Type::Info);
        mTeleported = true;
        if (mTpOnce.mValue) {
            mOnGroundPositions.clear();
        }
    }
}
