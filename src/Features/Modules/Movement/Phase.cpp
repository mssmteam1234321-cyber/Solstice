//
// 7/31/2024.
//

#include "Phase.hpp"

#include <Features/FeatureManager.hpp>
#include <Features/Events/BaseTickEvent.hpp>
#include <Features/Events/RunUpdateCycleEvent.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/World/Block.hpp>
#include <SDK/Minecraft/World/BlockLegacy.hpp>
#include <SDK/Minecraft/World/BlockSource.hpp>

std::vector<glm::ivec3> getCollidingBlocks()
{
    std::vector<glm::ivec3> collidingBlockList;
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return collidingBlockList;
    AABBShapeComponent* aabb = player->getAABBShapeComponent();
    glm::vec3 lower = aabb->mMin;
    glm::vec3 upper = aabb->mMin;
    //lower.x -= 0.1f;
    //lower.y -= 0.1f;
    //lower.z -= 0.1f;

    //upper.x += 0.1f;
    upper.y += aabb->mHeight;
    //upper.z += 0.1f;

    for (int x = floor(lower.x); x <= floor(upper.x); x++)
        for (int y = floor(lower.y); y <= floor(upper.y); y++)
            for (int z = floor(lower.z); z <= floor(upper.z); z++) {
                glm::ivec3 blockPos = { x, y, z };
                if (!BlockUtils::isAirBlock(blockPos)) {
                    collidingBlockList.emplace_back(blockPos);
                }
            }

    return collidingBlockList;
}

void Phase::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &Phase::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->listen<RunUpdateCycleEvent, &Phase::onRunUpdateCycleEvent>(this);
}

void Phase::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &Phase::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->deafen<RunUpdateCycleEvent, &Phase::onRunUpdateCycleEvent>(this);

    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;
    AABBShapeComponent* aabb = player->getAABBShapeComponent();
    if (aabb->mMax.y != aabb->mMin.y + aabb->mHeight) {
        aabb->mMax.y = aabb->mMin.y + aabb->mHeight;
    }

    mMoving = false;
}

void Phase::onBaseTickEvent(BaseTickEvent& event)
{
    auto player = event.mActor;
    AABBShapeComponent* aabb = player->getAABBShapeComponent();

    if (mMode.mValue == Mode::Horizontal) {
        aabb->mMax.y = aabb->mMin.y;
    }
    else if (mMode.mValue == Mode::Vertical) {
        auto moveInput = player->getMoveInputComponent();
        auto stateVector = player->getStateVectorComponent();
        bool isJumping = moveInput->mIsJumping;
        bool isSneaking = moveInput->mIsSneakDown;
        float value = 0;

        glm::vec3 belowBlockPos = *player->getPos();
        belowBlockPos.y -= (PLAYER_HEIGHT + 0.1f);
        glm::vec3 aboveBlockPos = *player->getPos();
        aboveBlockPos.y += 0.5f;

        std::vector<glm::ivec3> collidingBlocks = getCollidingBlocks();
        if (isJumping && (!BlockUtils::isAirBlock(aboveBlockPos) || !collidingBlocks.empty())) {
            value = mSpeed.mValue / 10;
            aabb->mMin.y += value;
            aabb->mMax.y += value;
            stateVector->mVelocity = { 0, 0, 0 };
        }else if (isSneaking && (!BlockUtils::isAirBlock(belowBlockPos) || !collidingBlocks.empty())) {
            value = -(mSpeed.mValue / 10);
            aabb->mMin.y += value;
            aabb->mMax.y += value;
            stateVector->mVelocity = { 0, 0, 0 };
            moveInput->mIsSneakDown = false;
        }

        mMoving = 0 < abs(value);
    }
}

void Phase::onRunUpdateCycleEvent(RunUpdateCycleEvent& event) 
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    if(mMoving && mBlink.mValue) event.mCancelled = true;
}