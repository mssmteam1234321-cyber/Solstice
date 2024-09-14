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
#include "Components/MobHurtTimeComponent.hpp"

#include <SDK/Minecraft/Inventory/ContainerManagerModel.hpp>
#include <SDK/Minecraft/World/Chunk/ChunkSource.hpp>


#define PLAYER_HEIGHT 1.62f
#define PLAYER_HEIGHT_VEC glm::vec3(0.f, PLAYER_HEIGHT, 0.f)

class Actor {
public:
    unsigned char filler[1032]; // From levilamina

    CLASS_FIELD(uintptr_t**, vtable, 0x0)
    CLASS_FIELD(EntityContext, mContext, 0x8);
    CLASS_FIELD(std::string, mEntityIdentifier, OffsetProvider::Actor_mEntityIdentifier);

    bool getStatusFlag(ActorFlags flag) {
        return MemUtils::callFastcall<bool>(SigManager::Actor_getStatusFlag, this, flag);
    }

    void setStatusFlag(ActorFlags flag, bool value) {
        //MemUtils::callFastcall<void>(SigManager::Actor_setStatusFlag, this, flag, value);
    }

    template<typename flag_t>
    bool getFlag() {
        auto storage = mContext.assure<flag_t>();
        return storage->contains(this->mContext.mEntityId);
    }

    template<typename flag_t>
    void setFlag(bool value)
    {
        try
        {
            auto storage = mContext.assure<flag_t>();
            bool has = storage->contains(this->mContext.mEntityId);
            if (value && !has) {
                storage->emplace(this->mContext.mEntityId);
            }
            else if (!value && has) {
                storage->remove(this->mContext.mEntityId);
            }
        } catch (std::exception& e) {
            spdlog::error("Failed to set flag: {}", e.what());
        } catch (...) {
            spdlog::error("Failed to set flag: unknown error");
        }
    }

    void swing();
    bool isDestroying();
    bool isSwinging();
    void setSwinging(bool swinging);
    int getGameType();
    void setGameType(int type);
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
    MobHurtTimeComponent* getMobHurtTimeComponent();
    class SimpleContainer* getArmorContainer();
    class PlayerInventory* getSupplies();
    class Level* getLevel();
    class SerializedSkin* getSkin();
    void setPosition(glm::vec3 pos);
    float distanceTo(Actor* actor);
    float distanceTo(const glm::vec3& pos);
    bool wasOnGround();
    bool isOnGround();
    void setOnGround(bool);
    bool isCollidingHorizontal();
    void jumpFromGround();
    float getFallDistance();
    void setFallDistance(float distance);
    std::string getRawName();
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
    std::string getLocalName();
    std::string getXuid();
    bool isValid();
};

static_assert(sizeof(Actor) == 0x408, "Actor size is invalid");