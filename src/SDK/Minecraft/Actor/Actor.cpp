//
// Created by vastrakai on 6/25/2024.
//

#include "Actor.hpp"

#include <SDK/OffsetProvider.hpp>
#include <SDK/SigManager.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/MinecraftSim.hpp>
#include <SDK/Minecraft/Network/LoopbackPacketSender.hpp>
#include <SDK/Minecraft/Network/MinecraftPackets.hpp>
#include <SDK/Minecraft/World/Level.hpp>
#include <Utils/MiscUtils/RenderUtils.hpp>

#include "ActorType.hpp"
#include "Components/ActorTypeComponent.hpp"
#include "Components/RuntimeIDComponent.hpp"
#include "Components/FlagComponent.hpp"
#include "Components/FallDistanceComponent.hpp"
#include "Components/ActorGameTypeComponent.hpp"
#include <SDK/Minecraft/Network/Packets/SetPlayerGameTypePacket.hpp>

#define COMPONENT_GET_FUNC(funcName, componentType) \
componentType* Actor::funcName() \
{ \
    if (!mContext.mRegistry->valid(this->mContext.mEntityId)) \
    { \
        spdlog::critical("Actor::{}: Invalid entity id", #funcName); \
        return nullptr; \
    } \
    auto component = mContext.getComponent<componentType>(); \
    if (!component) \
    { \
        spdlog::critical("Actor::{}: No {} found", #funcName, #componentType); \
    } \
    return component; \
}

COMPONENT_GET_FUNC(getWalkAnimationComponent, ActorWalkAnimationComponent)
COMPONENT_GET_FUNC(getActorTypeComponent, ActorTypeComponent)
COMPONENT_GET_FUNC(getRenderPositionComponent, RenderPositionComponent)
COMPONENT_GET_FUNC(getStateVectorComponent, StateVectorComponent)
COMPONENT_GET_FUNC(getMoveInputComponent, MoveInputComponent)
COMPONENT_GET_FUNC(getRawMoveInputComponent, RawMoveInputComponent)
COMPONENT_GET_FUNC(getActorRotationComponent, ActorRotationComponent)
COMPONENT_GET_FUNC(getAABBShapeComponent, AABBShapeComponent)
COMPONENT_GET_FUNC(getBlockMovementSlowdownMultiplierComponent, BlockMovementSlowdownMultiplierComponent)
COMPONENT_GET_FUNC(getActorUniqueIDComponent, ActorUniqueIDComponent)
COMPONENT_GET_FUNC(getActorHeadRotationComponent, ActorHeadRotationComponent)
COMPONENT_GET_FUNC(getMobBodyRotationComponent, MobBodyRotationComponent)
COMPONENT_GET_FUNC(getCameraDirectLookComponent, CameraDirectLookComponent)
COMPONENT_GET_FUNC(getMaxAutoStepComponent, MaxAutoStepComponent)
COMPONENT_GET_FUNC(getAttributesComponent, AttributesComponent)
COMPONENT_GET_FUNC(getCameraComponent, CameraComponent)

void Actor::swing()
{
    MemUtils::callVirtualFunc<void>(OffsetProvider::Actor_swing, this);
}

bool Actor::isDestroying()
{
    return hat::member_at<bool>(this, OffsetProvider::Actor_mDestroying);
}

bool Actor::isSwinging()
{
    return hat::member_at<bool>(this, OffsetProvider::Actor_mSwinging);
}

void Actor::setSwinging(bool swinging)
{
    hat::member_at<bool>(this, OffsetProvider::Actor_mSwinging) = swinging;
}

int Actor::getGameType()
{
    return mContext.getComponent<ActorGameTypeComponent>()->mGameType;
}

void Actor::setGameType(int type)
{
    auto pkt = MinecraftPackets::createPacket<SetPlayerGameTypePacket>();
    pkt->mPlayerGameType = static_cast<GameType>(type);
    PacketUtils::sendToSelf(pkt);
    ClientInstance::get()->getPacketSender()->sendToServer(pkt.get()); // on some servers this can server-side set the gamemode
}

bool Actor::isGameCameraActive()
{
    // use da flag component
    auto storage = mContext.assure<FlagComponent<GameCameraFlag>>();
    return storage->contains(this->mContext.mEntityId);
}

void Actor::setGameCameraActive(bool active)
{
    auto storage = mContext.assure<FlagComponent<GameCameraFlag>>();
    bool wasActive = isGameCameraActive();
    if (active && !wasActive)
    {
        storage->emplace(this->mContext.mEntityId);
    }
    else if (!active && wasActive)
    {
        storage->remove(this->mContext.mEntityId);
    }
}

bool Actor::isDebugCameraActive()
{
    // use da flag component
    auto storage = mContext.assure<DebugCameraIsActiveComponent>();
    return storage->contains(this->mContext.mEntityId);
}

void Actor::setDebugCameraActive(bool active)
{
    auto storage = mContext.assure<DebugCameraIsActiveComponent>();
    bool wasActive = isDebugCameraActive();
    if (active && !wasActive)
    {
        storage->emplace(this->mContext.mEntityId);
    }
    else if (!active && wasActive)
    {
        storage->remove(this->mContext.mEntityId);
    }
}

void Actor::setAllowInsideBlockRender(bool allow)
{
    auto storage = mContext.assure<FlagComponent<AllowInsideBlockRender>>();
    if (allow)
    {
        storage->emplace(this->mContext.mEntityId);
    }
    else
    {
        storage->remove(this->mContext.mEntityId);
    }

}

int Actor::getSwingProgress()
{
    return hat::member_at<int>(this, OffsetProvider::Actor_mSwinging + 0x4);
}

int Actor::getOldSwingProgress()
{
    return hat::member_at<int>(this, OffsetProvider::Actor_mSwinging + 0x8);
}

void Actor::setSwingProgress(int progress)
{
    hat::member_at<int>(this, OffsetProvider::Actor_mSwinging + 0x4) = progress;
}

AABB Actor::getAABB(bool normal)
{
    if (normal)
    {
        return AABB(this->getAABBShapeComponent()->mMin, this->getAABBShapeComponent()->mMax);
    }
    // Get all components
    auto renderPositionComponent = getRenderPositionComponent();
    if (!renderPositionComponent) return AABB();
    auto aabbShapeComponent = getAABBShapeComponent();
    if (!aabbShapeComponent) return AABB();

    glm::vec3 renderPos = renderPositionComponent->mPosition - glm::vec3(0, PLAYER_HEIGHT, 0);
    if (!isPlayer()) renderPos = renderPositionComponent->mPosition;

    glm::vec2 size = { aabbShapeComponent->mWidth, aabbShapeComponent->mHeight };
    auto localPlayer = ClientInstance::get()->getLocalPlayer();
    if (this == localPlayer)
    {
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

void Actor::setAABB(AABB& aabb)
{
    AABBShapeComponent* aabbShapeComponent = getAABBShapeComponent();
    if (!aabbShapeComponent) return;
    aabbShapeComponent->mMin = aabb.mMin;
    aabbShapeComponent->mMax = aabb.mMax;
}


bool Actor::isPlayer()
{
    auto actorType = getActorTypeComponent();
    if (!actorType) return false;
    return actorType->type == ActorType::Player;
}

ChunkPos Actor::getChunkPos()
{
    glm::vec3 pos = *getPos();
    pos.x = static_cast<int>(floor(pos.x));
    pos.y = static_cast<int>(floor(pos.y - (float)2.0f));
    pos.z = static_cast<int>(floor(pos.z));
    return ChunkPos(pos);
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

DebugCameraComponent* Actor::getDebugCameraComponent()
{
    return mContext.try_get<DebugCameraComponent>();
}

CameraPresetComponent* Actor::getCameraPresetComponent()
{
    return mContext.try_get<CameraPresetComponent>();
}

ContainerManagerModel* Actor::getContainerManagerModel()
{
    return hat::member_at<ContainerManagerModel*>(this, OffsetProvider::Actor_mContainerManagerModel);
}

JumpControlComponent* Actor::getJumpControlComponent()
{
    static auto func = SigManager::Mob_getJumpControlComponent;
    auto id = mContext.mEntityId;
    return MemUtils::callFastcall<JumpControlComponent*>(func, mContext.mRegistry, &id);

}

MobHurtTimeComponent* Actor::getMobHurtTimeComponent()
{
    return hat::member_at<MobHurtTimeComponent*>(this, OffsetProvider::Actor_mHurtTimeComponent);
}

SimpleContainer* Actor::getArmorContainer()
{
    return mContext.getComponent<ActorEquipmentComponent>()->mArmorContainer;
}

PlayerInventory* Actor::getSupplies()
{
    return hat::member_at<PlayerInventory*>(this, OffsetProvider::Actor_mSupplies);
}

Level* Actor::getLevel()
{
    return hat::member_at<Level*>(this, OffsetProvider::Actor_mLevel);
}

int64_t Actor::getRuntimeID()
{
    auto runtimeIDComponent = mContext.getComponent<RuntimeIDComponent>();
    return runtimeIDComponent ? runtimeIDComponent->runtimeID : -1;
}

void Actor::setPosition(glm::vec3 pos)
{
    static uintptr_t func = SigManager::Actor_setPosition;
    MemUtils::callFastcall<void, void*, glm::vec3*>(func, this, &pos);
}

float Actor::distanceTo(Actor* actor)
{
    glm::vec3* actorPos = actor->getPos();
    if (!actorPos) return -1;
    glm::vec3 closestPoint = getAABB().getClosestPoint(*actorPos);
    return distance(closestPoint, *actorPos);
}

float Actor::distanceTo(const glm::vec3& pos)
{
    glm::vec3 closestPoint = getAABB().getClosestPoint(pos);
    return distance(closestPoint, pos);
}

bool Actor::wasOnGround()
{
    auto storage = mContext.assure<FlagComponent<WasOnGroundFlag>>();
    return storage->contains(this->mContext.mEntityId);
}

bool Actor::isInWater()
{
    auto storage = mContext.assure<FlagComponent<InWaterFlag>>();
    return storage->contains(this->mContext.mEntityId);
}

void Actor::setInWater(bool inWater)
{
    auto storage = mContext.assure<FlagComponent<InWaterFlag>>();
    if (inWater)
    {
        storage->emplace(this->mContext.mEntityId);
    }
    else
    {
        storage->remove(this->mContext.mEntityId);
    }
}

bool Actor::isOnGround()
{
    auto storage = mContext.assure<OnGroundFlagComponent>();
    return storage->contains(this->mContext.mEntityId);
}

void Actor::setOnGround(bool flag)
{
    auto storage = mContext.assure<OnGroundFlagComponent>();
    auto wasOnGround = isOnGround();
    if (flag && !wasOnGround)
    {
        storage->emplace(this->mContext.mEntityId);
    }
    else if (!flag && wasOnGround)
    {
        storage->remove(this->mContext.mEntityId);
    }
}


void Actor::jumpFromGround()
{
    auto jumpComponent = getJumpControlComponent();
    bool noJumpDelay = jumpComponent->mNoJumpDelay;
    jumpComponent->mNoJumpDelay = false;
    static auto func = reinterpret_cast<void*>(SigManager::SimulatedPlayer_simulateJump);
    MemUtils::callFastcall<void>(func, this);
    jumpComponent->mNoJumpDelay = noJumpDelay;
}

float Actor::getFallDistance()
{
    return mContext.getComponent<FallDistanceComponent>()->mFallDistance;
}

std::string Actor::getRawName()
{
    auto actorType = getActorTypeComponent();
    if (!actorType)
    {
        spdlog::warn("Actor::getRawName: No ActorTypeComponent found");
        return "Unknown?";
    }
    auto actorUniqueID = getActorUniqueIDComponent();
    if (!actorUniqueID)
    {
        spdlog::warn("Actor::getRawName: No ActorUniqueIDComponent found");
        return "What?";
    }

    if (actorType->type != ActorType::Player)
    {
        // mEntityIdentifier will be used as fallback
        return mEntityIdentifier;
    }
    auto player = ClientInstance::get()->getLocalPlayer();
    uint64_t uniqueId = actorUniqueID->mUniqueID;

    for (auto& [id, entry] : *player->getLevel()->getPlayerList())
    {
        if (entry.id == uniqueId)
        {
            return entry.name;
        }
    }

    std::string name = getNameTag();
    name = name.substr(0, name.find('\n'));
    name = ColorUtils::removeColorCodes(name);
    return name;
}

const std::string& Actor::getNameTag()
{
    static auto func = SigManager::Actor_getNameTag;
    return *MemUtils::callFastcall<std::string*>(func, this);
}

void Actor::setNametag(const std::string& name)
{
    static auto func = SigManager::Actor_setNameTag;
    MemUtils::callFastcall<void, void*, std::string>(func, this, name);
}

float Actor::getMaxHealth()
{
    auto health = getAttribute(Health);
    if (!health) return 0;
    return health->maximumValue;
}

float Actor::getHealth()
{
    auto health = getAttribute(Health);
    if (!health) return 0;
    return health->currentValue;
}

float Actor::getAbsorption()
{
    auto absorption = getAttribute(Absorption);
    if (!absorption) return 0;
    return absorption->currentValue;
}

float Actor::getMaxAbsorption()
{
    auto absorption = getAttribute(Absorption);
    if (!absorption) return 0;
    return absorption->maximumValue;
}

AttributeInstance* Actor::getAttribute(AttributeId id)
{
    return getAttribute(static_cast<int>(id));
}

AttributeInstance* Actor::getAttribute(int id)
{
    auto component = getAttributesComponent();
    if (!component) {
        spdlog::critical("Actor::getAttribute: No AttributesComponent found");
        return nullptr;
    }
    // Directly access the map
    auto& map = component->baseAttributeMap.attributes;
    auto it = map.find(id);
    if (it != map.end()) {
        return &it->second;
    }
    return nullptr;
}

bool Actor::isOnFire()
{
    return getFlag<OnFireComponent, false>();
}
