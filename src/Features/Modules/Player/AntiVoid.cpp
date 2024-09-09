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
    mLastOnGroundPos = *player->getPos();
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
        mLastOnGroundPos = BlockUtils::findClosestBlockToPos(*player->getPos());
        mTeleported = false;
        return;
    }

    bool hasTeleported = mTeleported && mTpOnce.mValue;

    if (player->getFallDistance() > mFallDistance.mValue && (!hasTeleported || !mTpOnce.mValue))
    {
        if (mLastOnGroundPos == glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX))
        {
            NotifyUtils::notify("Couldn't find a block to teleport to!", 5.0f, Notification::Type::Error);
            return;
        }

        /*// If the block below the pos is air, we need to find the closest block to the player
        if (BlockUtils::isAirBlock(mLastOnGroundPos))
        {
            mLastOnGroundPos = BlockUtils::findClosestBlockToPos(*player->getPos());
        }
        */

        mLastOnGroundPos = mLastOnGroundPos + glm::vec3(0.f, 1.f + PLAYER_HEIGHT + 0.01, 0.f);

        player->setPosition(glm::ivec3(mLastOnGroundPos));
        player->setFallDistance(0.0f);
        NotifyUtils::notify("Teleported!", 5.0f, Notification::Type::Info);
        mTeleported = true;
    }
}
