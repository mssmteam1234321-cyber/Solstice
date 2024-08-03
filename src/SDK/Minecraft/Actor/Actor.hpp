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
#include "Components/AttributesComponent.hpp"
#include "Components/CameraComponent.hpp"
#include "Components/CameraPresetComponent.hpp"
#include "Components/ActorWalkAnimationComponent.hpp"
#include "Components/ActorTypeComponent.hpp"
#include "Components/FlagComponent.hpp"
#include "Components/MaxAutoStepComponent.hpp"

#include <SDK/Minecraft/Inventory/ContainerManagerModel.hpp>
#include <SDK/Minecraft/World/Chunk/ChunkSource.hpp>


#define PLAYER_HEIGHT 1.62f
#define PLAYER_HEIGHT_VEC glm::vec3(0.f, PLAYER_HEIGHT, 0.f)

class Actor {
public:
    CLASS_FIELD(uintptr_t**, vtable, 0x0)
    CLASS_FIELD(EntityContext, mContext, 0x8);
    CLASS_FIELD(std::string, mEntityIdentifier, 0x2B8)

    virtual bool getStatusFlag(ActorFlags) = 0;
    virtual void setStatusFlag(ActorFlags, bool) = 0;

    template<typename flag_t, bool real_flag = true>
    bool getFlag() {
        if (real_flag)
        {
            auto storage = mContext.assure<FlagComponent<flag_t>>();
            return storage->contains(this->mContext.mEntityId);
        }

        auto storage = mContext.assure<flag_t>();
        return storage->contains(this->mContext.mEntityId);
    }

    template<typename flag_t, bool real_flag = true>
    void setFlag(bool value) {
        if (!real_flag)
        {
            auto storage = mContext.assure<flag_t>();
            bool has = storage->contains(this->mContext.mEntityId);
            if (value && !has) {
                storage->emplace(this->mContext.mEntityId);
            }
            else if (!value && has) {
                storage->remove(this->mContext.mEntityId);
            }
            return;
        }

        auto storage = mContext.assure<FlagComponent<flag_t>>();
        bool has = storage->contains(this->mContext.mEntityId);
        if (value && !has) {
            storage->emplace(this->mContext.mEntityId);
        }
        else if (!value && has) {
            storage->remove(this->mContext.mEntityId);
        }
    }

    void swing();
    bool isDestroying();
    bool isSwinging();
    void setSwinging(bool swinging);
    int getGameType();
    void setGameType(int type);
    bool isGameCameraActive();
    void setGameCameraActive(bool active);
    bool isDebugCameraActive();
    void setDebugCameraActive(bool active);
    void setAllowInsideBlockRender(bool allow);
    int getSwingProgress();
    int getOldSwingProgress();
    void setSwingProgress(int progress);
    AABB getAABB(bool normal = false);
    void setAABB(AABB& aabb);
    bool isPlayer();
    ChunkPos getChunkPos();
    glm::vec3* getPos();
    glm::vec3* getPosPrev();
    class GameMode* getGameMode();
    ActorWalkAnimationComponent* getWalkAnimationComponent();
    DebugCameraComponent* getDebugCameraComponent();
    CameraPresetComponent* getCameraPresetComponent();
    ActorTypeComponent* getActorTypeComponent();
    RenderPositionComponent* getRenderPositionComponent();
    int64_t getRuntimeID();
    StateVectorComponent* getStateVectorComponent();
    MoveInputComponent* getMoveInputComponent();
    RawMoveInputComponent* getRawMoveInputComponent();
    ActorRotationComponent* getActorRotationComponent();
    AABBShapeComponent* getAABBShapeComponent();
    BlockMovementSlowdownMultiplierComponent* getBlockMovementSlowdownMultiplierComponent();
    ActorUniqueIDComponent* getActorUniqueIDComponent();
    ContainerManagerModel* getContainerManagerModel();
    ActorHeadRotationComponent* getActorHeadRotationComponent();
    MobBodyRotationComponent* getMobBodyRotationComponent();
    JumpControlComponent* getJumpControlComponent();
    CameraComponent* getCameraComponent();
    CameraDirectLookComponent* getCameraDirectLookComponent();
    MaxAutoStepComponent* getMaxAutoStepComponent();
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
    const std::string& getRawName();
    const std::string& getNameTag();
    void setNametag(const std::string& name);
    AttributesComponent* getAttributesComponent();
    float getMaxHealth();
    float getHealth();
    float getAbsorption();
    float getMaxAbsorption();
    AttributeInstance* getAttribute(AttributeId id);
    AttributeInstance* getAttribute(int id);
    bool isOnFire();
};
