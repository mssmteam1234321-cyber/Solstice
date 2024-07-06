//
// Created by vastrakai on 6/25/2024.
//

#include "Actor.hpp"

#include <SDK/OffsetProvider.hpp>
#include <SDK/SigManager.hpp>

#include "Components/RuntimeIDComponent.hpp"

glm::vec3* Actor::getPos()
{
    return &getStateVectorComponent()->mPos;
}

glm::vec3* Actor::getPosPrev()
{
    return &getStateVectorComponent()->mPosOld;
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

BlockMovementSlowdownMultiplierComponent* Actor::getBlockMovementSlowdownMultiplierComponent()
{
    return mContext.getComponent<BlockMovementSlowdownMultiplierComponent>();
}

ContainerManagerModel* Actor::getContainerManagerModel()
{
    return hat::member_at<ContainerManagerModel*>(this, OffsetProvider::Actor_mContainerManagerModel);
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
