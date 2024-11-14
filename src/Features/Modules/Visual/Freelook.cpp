//
// Created by vastrakai on 11/2/2024.
//

#include "Freelook.hpp"

#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Options.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/Actor/Components/CameraComponent.hpp>
#include <SDK/Minecraft/Actor/Components/ItemUseSlowdownModifierComponent.hpp>

std::vector<unsigned char> gFrlBytes = { 0xC3, 0x90, 0x90 };
DEFINE_PATCH_FUNC(Freelook::patchUpdates, SigManager::Unknown_updatePlayerFromCamera, gFrlBytes);

void Freelook::onEnable()
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player)
    {
        setEnabled(false);
        return;
    }

    return;

    auto rotc = player->getActorRotationComponent();

    mLookingAngles = {rotc->mPitch, rotc->mYaw};
    mLastCameraState = ClientInstance::get()->getOptions()->mThirdPerson->value;
    ClientInstance::get()->getOptions()->mThirdPerson->value = 1;

    auto storage = player->mContext.assure<UpdatePlayerFromCameraComponent>();

    for (std::tuple<EntityId, UpdatePlayerFromCameraComponent&> entt : storage->each())
    {
        EntityId id = std::get<0>(entt);
        int mode = std::get<1>(entt).mUpdateMode;

        mCameras[id] = mode;
    }



    storage->clear();

    patchUpdates(true);
}

void Freelook::onDisable()
{
    patchUpdates(false);

    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player)
    {
        mCameraDirectLookComponents.clear();
        mOriginalRots.clear();
        return;
    }

    for (auto&& [id, cameraComponent] : player->mContext.mRegistry->view<CameraComponent>().each())
    {
        CameraOrbitComponent* camera = player->mContext.mRegistry->try_get<CameraOrbitComponent>(id);

    }


    return;

    auto storage = player->mContext.assure<UpdatePlayerFromCameraComponent>();

    for (auto& [id, mode] : mCameras)
    {
        storage->emplace(id, UpdatePlayerFromCameraComponent(mode));
    }

    auto options = ClientInstance::get()->getOptions();
    options->mThirdPerson->value = mLastCameraState;


}
