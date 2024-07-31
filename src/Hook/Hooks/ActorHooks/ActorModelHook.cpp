//
// Created by vastrakai on 7/25/2024.
//

#include "ActorModelHook.hpp"

#include <SDK/OffsetProvider.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/Actor/ActorPartModel.hpp>
#include <Features/Events/BoneRenderEvent.hpp>
#include <Features/Modules/Visual/Animations.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>

std::unique_ptr<Detour> ActorModelHook::mDetour;

//void ActorAnimationControllerPlayer::applyToPose(RenderParams&, std::unordered_map<SkeletalHierarchyIndex,std::vector<BoneOrientation>>&, float)
void ActorModelHook::onActorModel(uintptr_t a1, uintptr_t a2, uintptr_t a3, float a4, int a5)
{
    auto original = mDetour->getOriginal<decltype(&ActorModelHook::onActorModel)>();
    original(a1, a2, a3, a4, a5);

    auto ent = *reinterpret_cast<Actor**>(a2 + 0x38); // 0x38 is the pointer to actor :D
    if (!ent) return;

    const auto bone = reinterpret_cast<Bone*>(a3);
    const auto partModel = bone->getActorPartModel();

    if (ent == ClientInstance::get()->getLocalPlayer() && ClientInstance::get())
    {
        if (bone->mBoneStr == "rightarm")
        {
            static auto animMod = gFeatureManager->mModuleManager->getModule<Animations>();
            if (animMod->mThirdPersonBlock && animMod->mShouldBlock)
            {
                static float xRot = -65.f;
                static float yRot = -17.f;
                static float zRot = 57.f;

                Actor* player = ClientInstance::get()->getLocalPlayer();

                if (player->getSwingProgress() > 0) {
                    xRot = MathUtils::animate(-30, xRot, ImRenderUtils::getDeltaTime() * 3.f);
                    yRot = MathUtils::animate(0, yRot, ImRenderUtils::getDeltaTime() * 3.f);
                }
                else {
                    xRot = MathUtils::animate(-65, xRot, ImRenderUtils::getDeltaTime() * 3.f);
                    yRot = MathUtils::animate(-17, yRot, ImRenderUtils::getDeltaTime() * 3.f);
                }

                partModel->mRot.x = xRot;
                partModel->mRot.y = yRot;
                partModel->mRot.z = zRot;
                partModel->mSize.z = 0.9177761;
            }
        }
    }

    auto holder = nes::make_holder<BoneRenderEvent>(bone, partModel, ent);
    gFeatureManager->mDispatcher->trigger(holder);

    /*if (bone->mBoneStr == "rightarm")
    {
        partModel->mPos.x = -4.041;
        partModel->mPos.y = -2.798;
        partModel->mPos.z = -0.622;
        partModel->mRot.x = -58.031;
        partModel->mRot.y = -53.886;
        partModel->mRot.z = 14.508;
    }*/


}

void ActorModelHook::init()
{
    mDetour = std::make_unique<Detour>("ActorAnimationControllerPlayer::applyToPose", reinterpret_cast<void*>(SigManager::ActorAnimationControllerPlayer_applyToPose), &ActorModelHook::onActorModel);
}