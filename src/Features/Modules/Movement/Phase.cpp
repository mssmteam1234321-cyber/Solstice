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
    float width = aabb->mWidth - 0.2f;
    glm::vec3 lower = aabb->mMin;
    glm::vec3 upper = aabb->mMin;
    lower.x -= width / 2.f;
    lower.y -= 0.1f;
    lower.z -= width / 2.f;

    upper.x += width / 2.f;
    upper.y += aabb->mHeight + 1.f;
    upper.z += width / 2.f;

    for (int x = floor(lower.x); x <= floor(upper.x); x++)
        for (int y = floor(lower.y); y <= floor(upper.y); y++)
            for (int z = floor(lower.z); z <= floor(upper.z); z++) {
                glm::ivec3 blockPos = { x, y, z };
                if (!ClientInstance::get()->getBlockSource()->getBlock(blockPos)->mLegacy->isAir()) {
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
        glm::vec3 motion = { 0, 0, 0 };

        std::vector<glm::ivec3> collidingBlocks = getCollidingBlocks();

        if (!collidingBlocks.empty()) {
            if ((isJumping || isSneaking)) motion.y = isJumping ? mSpeed.mValue / 10 : -(mSpeed.mValue / 10);
            aabb->mMax.y = 0.f;
            stateVector->mVelocity = motion;

            glm::vec3 belowBlockPos = *player->getPos();
            belowBlockPos.y -= (PLAYER_HEIGHT + 0.1f);
            player->setOnGround(!ClientInstance::get()->getBlockSource()->getBlock(belowBlockPos)->mLegacy->isAir());
        }
        else {
            if (0 < stateVector->mVelocity.y) stateVector->mVelocity.y = 0.f;

            if (aabb->mMax.y != aabb->mMin.y + aabb->mHeight) {
                aabb->mMax.y = aabb->mMin.y + aabb->mHeight;
            }
        }

        mMoving = 0 < abs(motion.y);
    }
}

void Phase::onRunUpdateCycleEvent(RunUpdateCycleEvent& event) 
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    if(mMoving && mBlink.mValue) event.mCancelled = true;
}