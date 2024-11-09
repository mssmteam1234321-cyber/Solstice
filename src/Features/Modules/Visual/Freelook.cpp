//
// Created by vastrakai on 11/2/2024.
//

#include "Freelook.hpp"

#include <SDK/Minecraft/Actor/Components/CameraComponent.hpp>

// TODO: Figure out why emplace

void Freelook::onEnable()
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player)
    {
        setEnabled(false);
        return;
    }

    auto rotc = player->getActorRotationComponent();

    mLookingAngles = {rotc->mPitch, rotc->mYaw};
    mLastCameraState = ClientInstance::get()->getOptions()->mThirdPerson->value;
    ClientInstance::get()->getOptions()->mThirdPerson->value = 1;

    entt::basic_storage<UpdatePlayerFromCameraComponent, EntityId>* storage = player->mContext.assure<
        UpdatePlayerFromCameraComponent>();
    // gather all of the Entity IDS that have the UpdatePlayerFromCameraComponent
    std::set<EntityId> cameras;

    for (std::tuple<EntityId, UpdatePlayerFromCameraComponent&> entt : storage->each())
    {
        // Get the entity id
        EntityId id = std::get<0>(entt);
        cameras.insert(id);
    }
    storage->clear();
}

void Freelook::onDisable()
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    auto options = ClientInstance::get()->getOptions();
    options->mThirdPerson->value = mLastCameraState;

    auto rotc = player->getActorRotationComponent();
    rotc->mPitch = mLookingAngles.x;
    rotc->mYaw = mLookingAngles.y;

    const auto storage = player->mContext.assure<UpdatePlayerFromCameraComponent>();
    for (EntityId id : mCameras)
    {
        // replace the UpdatePlayerFromCameraComponent on the camera...
        storage->emplace(id);
    }
    mCameras.clear();
}
