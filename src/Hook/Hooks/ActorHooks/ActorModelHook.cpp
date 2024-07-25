//
// Created by vastrakai on 7/25/2024.
//

#include "ActorModelHook.hpp"

#include <SDK/OffsetProvider.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/Actor/ActorPartModel.hpp>
#include <Features/Events/BoneRenderEvent.hpp>

std::unique_ptr<Detour> ActorModelHook::mDetour;


void ActorModelHook::onActorModel(uintptr_t a1, uintptr_t a2, uintptr_t a3, float a4, int a5)
{
    auto original = mDetour->getOriginal<decltype(&ActorModelHook::onActorModel)>();
    original(a1, a2, a3, a4, a5);

    auto ent = *reinterpret_cast<Actor**>(a2 + OffsetProvider::Unknown_mActor);
    if (!ent) return;

    const auto bone = reinterpret_cast<Bone*>(a3);
    const auto partModel = bone->getActorPartModel();

    auto holder = nes::make_holder<BoneRenderEvent>(bone, partModel, ent);
    gFeatureManager->mDispatcher->trigger(holder);

    // if (bone->mBoneStr == "rightarm")
    // {
    //     partModel->mPos.x = -4.041;
    //     partModel->mPos.y = -2.798;
    //     partModel->mPos.z = -0.622;
    //     partModel->mRot.x -= -58.031;
    //     partModel->mRot.y -= -53.886;
    //     partModel->mRot.z -= 14.508;
    // }


}

void ActorModelHook::init()
{
    mDetour = std::make_unique<Detour>("ActorModel::[unknown]", reinterpret_cast<void*>(SigManager::ActorModel), &ActorModelHook::onActorModel);
}