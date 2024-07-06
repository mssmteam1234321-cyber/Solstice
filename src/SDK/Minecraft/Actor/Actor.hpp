//
// Created by vastrakai on 6/25/2024.
//
#pragma once

#include <Utils/MemUtils.hpp>
#include "EntityContext.hpp"
#include "glm/vec3.hpp"
#include "ActorFlags.hpp"
#include "Components/StateVectorComponent.hpp"
#include "Components/MoveInputComponent.hpp"
#include "Components/ActorRotationComponent.hpp"
#include "Components/BlockMovementSlowdownMultiplierComponent.hpp"
#include "Components/ActorEquipmentComponent.hpp"
#include <SDK/Minecraft/Inventory/ContainerManagerModel.hpp>


#define PLAYER_HEIGHT 1.62f

class Actor {
public:
    CLASS_FIELD(uintptr_t**, vtable, 0x0)
    CLASS_FIELD(EntityContext, mContext, 0x8);

    virtual bool getStatusFlag(ActorFlags) = 0;
    virtual void setStatusFlag(ActorFlags, bool) = 0;

    glm::vec3* getPos();
    glm::vec3* getPosPrev();
    class GameMode* getGameMode();
    int64_t getRuntimeID();
    StateVectorComponent* getStateVectorComponent();
    MoveInputComponent* getMoveInputComponent();
    ActorRotationComponent* getActorRotationComponent();
    BlockMovementSlowdownMultiplierComponent* getBlockMovementSlowdownMultiplierComponent();
    ContainerManagerModel* getContainerManagerModel();
    class SimpleContainer* getArmorContainer();
    class PlayerInventory* getSupplies();
    void setPosition(glm::vec3 pos);
};
