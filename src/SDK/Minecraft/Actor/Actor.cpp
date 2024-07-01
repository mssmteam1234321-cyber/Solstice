//
// Created by vastrakai on 6/25/2024.
//

#include "Actor.hpp"

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
