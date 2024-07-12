//
// Created by vastrakai on 6/25/2024.
//
#pragma once

#include <Utils/MemUtils.hpp>
#include "EntityContext.hpp"
#include <glm/glm.hpp>
#include "ActorFlags.hpp"
#include "Components/StateVectorComponent.hpp"
#include "Components/MoveInputComponent.hpp"
#include "Components/ActorRotationComponent.hpp"
#include "Components/BlockMovementSlowdownMultiplierComponent.hpp"
#include "Components/ActorEquipmentComponent.hpp"
#include "Components/AABBShapeComponent.hpp"
#include "Components/RenderPositionComponent.hpp"
#include "Components/ActorUniqueIDComponent.hpp"
#include "Components/ActorHeadRotationComponent.hpp"
#include "Components/MobBodyRotationComponent.hpp"
#include "Components/JumpControlComponent.hpp"
#include <SDK/Minecraft/Inventory/ContainerManagerModel.hpp>

#include "Components/ActorTypeComponent.hpp"


#define PLAYER_HEIGHT 1.62f

class Actor {
public:
    CLASS_FIELD(uintptr_t**, vtable, 0x0)
    CLASS_FIELD(EntityContext, mContext, 0x8);
    CLASS_FIELD(std::string, mEntityIdentifier, 0x2B8)

    virtual bool getStatusFlag(ActorFlags) = 0;
    virtual void setStatusFlag(ActorFlags, bool) = 0;

    void swing();
    bool isSwinging();
    void setSwinging(bool swinging);
    int getSwingProgress();
    void setSwingProgress(int progress);
    AABB getAABB();
    bool isPlayer();
    glm::vec3* getPos();
    glm::vec3* getPosPrev();
    class GameMode* getGameMode();
    ActorTypeComponent* getActorTypeComponent();
    RenderPositionComponent* getRenderPositionComponent();
    int64_t getRuntimeID();
    StateVectorComponent* getStateVectorComponent();
    MoveInputComponent* getMoveInputComponent();
    ActorRotationComponent* getActorRotationComponent();
    AABBShapeComponent* getAABBShapeComponent();
    BlockMovementSlowdownMultiplierComponent* getBlockMovementSlowdownMultiplierComponent();
    ActorUniqueIDComponent* getActorUniqueIDComponent();
    ContainerManagerModel* getContainerManagerModel();
    ActorHeadRotationComponent* getActorHeadRotationComponent();
    MobBodyRotationComponent* getMobBodyRotationComponent();
    JumpControlComponent* getJumpControlComponent();
    class SimpleContainer* getArmorContainer();
    class PlayerInventory* getSupplies();
    class Level* getLevel();
    void setPosition(glm::vec3 pos);
    float distanceTo(Actor* actor);
    bool wasOnGround();
    bool isInWater();
    void setInWater(bool inWater);
    bool isOnGround();
    void setOnGround(bool);
    void jumpFromGround();
    float getFallDistance();
};
