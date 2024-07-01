//
// Created by vastrakai on 6/25/2024.
//
#pragma once

#include <Utils/MemUtils.hpp>
#include "EntityContext.hpp"
#include "glm/vec3.hpp"
#include "Components/StateVectorComponent.hpp"
#include "Components/MoveInputComponent.hpp"
#include "Components/ActorRotationComponent.hpp"

class Actor {
public:
    CLASS_FIELD(uintptr_t**, vtable, 0x0);
    CLASS_FIELD(EntityContext, mContext, 0x8);

    glm::vec3* getPos();
    glm::vec3* getPosPrev();
    StateVectorComponent* getStateVectorComponent();
    MoveInputComponent* getMoveInputComponent();
    ActorRotationComponent* getActorRotationComponent();
};
