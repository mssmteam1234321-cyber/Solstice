//
// Created by vastrakai on 6/25/2024.
//

#include "Actor.hpp"

#include <SDK/OffsetProvider.hpp>
#include <SDK/SigManager.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/MinecraftSim.hpp>
#include <Utils/MiscUtils/RenderUtils.hpp>

#include "ActorType.hpp"
#include "Components/ActorTypeComponent.hpp"
#include "Components/RuntimeIDComponent.hpp"

AABB Actor::getAABB()
{
    glm::vec3 renderPos = getRenderPositionComponent()->mPosition - glm::vec3(0, PLAYER_HEIGHT, 0);
    if (!isPlayer()) renderPos = getRenderPositionComponent()->mPosition;

    glm::vec2 size = { getAABBShapeComponent()->mWidth, getAABBShapeComponent()->mHeight };
    auto localPlayer = ClientInstance::get()->getLocalPlayer();
    if (this == localPlayer)
    {
        /*glm::vec3 pos = *getPos() - glm::vec3(0, PLAYER_HEIGHT, 0);
        glm::vec3 posOld = *getPosPrev() - glm::vec3(0, PLAYER_HEIGHT, 0);
        */

        renderPos = RenderUtils::transform.mPlayerPos - glm::vec3(0, PLAYER_HEIGHT, 0);
    }

    float hitboxWidth = size.x;
    float hitboxHeight = size.y;
    glm::vec3 aabbMin = renderPos - glm::vec3(hitboxWidth / 2, 0, hitboxWidth / 2);
    glm::vec3 aabbMax = renderPos + glm::vec3(hitboxWidth / 2, hitboxHeight, hitboxWidth / 2);
    aabbMin = aabbMin - glm::vec3(0.1f, 0.1f, 0.1f);
    aabbMax = aabbMax + glm::vec3(0.1f, 0.1f, 0.1f);
    return AABB(aabbMin, aabbMax, true);
}


bool Actor::isPlayer()
{
    auto actorType = getActorTypeComponent();
    if (!actorType) return false;
    return actorType->type == ActorType::Player;
}

glm::vec3* Actor::getPos()
{
    return &getStateVectorComponent()->mPos;
}

glm::vec3* Actor::getPosPrev()
{
    return &getStateVectorComponent()->mPosOld;
}

GameMode* Actor::getGameMode()
{
    return hat::member_at<GameMode*>(this, OffsetProvider::Actor_mGameMode);
}

ActorTypeComponent* Actor::getActorTypeComponent()
{
    return mContext.getComponent<ActorTypeComponent>();
}

RenderPositionComponent* Actor::getRenderPositionComponent()
{
    return mContext.getComponent<RenderPositionComponent>();
}

StateVectorComponent* Actor::getStateVectorComponent()
{
    return mContext.getComponent<StateVectorComponent>();
}

MoveInputComponent* Actor::getMoveInputComponent()
{
    return mContext.getComponent<MoveInputComponent>();
}

ActorRotationComponent* Actor::getActorRotationComponent()
{
    return mContext.getComponent<ActorRotationComponent>();
}

AABBShapeComponent* Actor::getAABBShapeComponent()
{
    return mContext.getComponent<AABBShapeComponent>();
}

BlockMovementSlowdownMultiplierComponent* Actor::getBlockMovementSlowdownMultiplierComponent()
{
    return mContext.getComponent<BlockMovementSlowdownMultiplierComponent>();
}

ContainerManagerModel* Actor::getContainerManagerModel()
{
    return hat::member_at<ContainerManagerModel*>(this, OffsetProvider::Actor_mContainerManagerModel);
}

SimpleContainer* Actor::getArmorContainer()
{
    return mContext.getComponent<ActorEquipmentComponent>()->mArmorContainer;
}

PlayerInventory* Actor::getSupplies()
{
    return hat::member_at<PlayerInventory*>(this, OffsetProvider::Actor_mSupplies);
}

int64_t Actor::getRuntimeID()
{
    return mContext.getComponent<RuntimeIDComponent>()->runtimeID;
}

void Actor::setPosition(glm::vec3 pos)
{
    static uintptr_t func = SigManager::Actor_setPosition;
    MemUtils::callFastcall<void, void*, glm::vec3*>(func, this, &pos);
}
