//
// Created by vastrakai on 11/2/2024.
//

#include "Freelook.hpp"

#include <SDK/Minecraft/Actor/Components/CameraComponent.hpp>
#include <SDK/Minecraft/Actor/Components/ItemUseSlowdownModifierComponent.hpp>

std::vector<unsigned char> gFrlBytes = { 0xC3, 0x90, 0x90 };
DEFINE_PATCH_FUNC(Freelook::patchUpdates, SigManager::Unknown_updatePlayerFromCamera, gFrlBytes);

class CameraOrbitComponent
{
    glm::vec2 mRotRads{};
    float mDelta = 0.0f;
    glm::vec2 mWrap{};
};

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

    auto storage = player->mContext.assure<CameraDirectLookComponent>();

    for (std::tuple<EntityId, CameraDirectLookComponent&> entt : storage->each())
    {
        EntityId id = std::get<0>(entt);

        spdlog::info("Entity ID: {} - Component: {:X}", id, reinterpret_cast<uintptr_t>(&std::get<1>(entt)));
        mCameraDirectLookComponents.push_back(&std::get<1>(entt));
        mOriginalRots[&std::get<1>(entt)] = std::get<1>(entt);
    }

    {
        auto s = player->mContext.assure<ItemUseSlowdownModifierComponent>();

        for (std::tuple<EntityId, ItemUseSlowdownModifierComponent&> entt : s->each())
        {
            EntityId id = std::get<0>(entt);

            spdlog::info("Entity ID: {} - Component: {:X}", id, reinterpret_cast<uintptr_t>(&std::get<1>(entt)));
        }


    }

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

    auto storage = player->mContext.assure<CameraDirectLookComponent>();

    for (std::tuple<EntityId, CameraDirectLookComponent&> entt : storage->each())
    {
        // Get the entity id
        EntityId id = std::get<0>(entt);
        // Log the entity id and their component
        spdlog::info("Entity ID: {} - Component: {:X}", id, reinterpret_cast<uintptr_t>(&std::get<1>(entt)));
        auto comp = &std::get<1>(entt);
        *comp = mOriginalRots[comp];
    }

    mCameraDirectLookComponents.clear();
    mOriginalRots.clear();

    auto options = ClientInstance::get()->getOptions();
    options->mThirdPerson->value = mLastCameraState;


}
