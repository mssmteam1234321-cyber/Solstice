//
// Created by vastrakai on 7/18/2024.
//

#include "Animations.hpp"

#include <Features/Events/BaseTickEvent.hpp>
#include <Features/Events/BobHurtEvent.hpp>
#include <Features/Events/SwingDurationEvent.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/MinecraftSim.hpp>
#include <SDK/Minecraft/Inventory/PlayerInventory.hpp>

DEFINE_NOP_PATCH_FUNC(patchNoSwitchAnimation, SigManager::ItemInHandRenderer_render_bytepatch, 0x4);
DEFINE_NOP_PATCH_FUNC(patchFluxSwing, SigManager::FluxSwing, 0x5);
DEFINE_NOP_PATCH_FUNC(patchDefaultSwing, SigManager::ItemInHandRenderer_renderItem_bytepatch, 0x8);
DEFINE_NOP_PATCH_FUNC(patchDefaultSwing2, SigManager::ItemInHandRenderer_renderItem_bytepatch+11, 0x8);

void Animations::onEnable()
{
    gFeatureManager->mDispatcher->listen<SwingDurationEvent, &Animations::onSwingDurationEvent>(this);
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &Animations::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->listen<BobHurtEvent, &Animations::onBobHurtEvent>(this);

    if (!mSwingAngle)
    {
        mSwingAngle = reinterpret_cast<float*>(SigManager::TapSwingAnim);
        MemUtils::setProtection(reinterpret_cast<uintptr_t>(mSwingAngle), sizeof(float), PAGE_READWRITE);
    }

    patchNoSwitchAnimation(mNoSwitchAnimation.mValue);
    patchFluxSwing(mFluxSwing.mValue);
    patchDefaultSwing(mAnimation.mValue == Animation::Test);
    patchDefaultSwing2(mAnimation.mValue == Animation::Test);
}

void Animations::onDisable()
{
    gFeatureManager->mDispatcher->deafen<SwingDurationEvent, &Animations::onSwingDurationEvent>(this);
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &Animations::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->deafen<BobHurtEvent, &Animations::onBobHurtEvent>(this);
    patchNoSwitchAnimation(false);
    patchFluxSwing(false);

    if (mSwingAngle) *mSwingAngle = -80.f;

    patchDefaultSwing(false);
    patchDefaultSwing2(false);
}


void Animations::onBaseTickEvent(BaseTickEvent& event)
{
    patchNoSwitchAnimation(mNoSwitchAnimation.mValue);
    patchFluxSwing(mFluxSwing.mValue);
    patchDefaultSwing(mAnimation.mValue == Animation::Test);
    patchDefaultSwing2(mAnimation.mValue == Animation::Test);

    *mSwingAngle = mCustomSwingAngle.mValue ? mSwingAngleSetting.as<float>() : -80.f;

    auto player = event.mActor;

    mOldSwingDuration = mSwingDuration;
    mSwingDuration = player->getSwingProgress();

}

void Animations::onSwingDurationEvent(SwingDurationEvent& event)
{
    event.mSwingDuration = mSwingTime.as<int>();
}

void Animations::onBobHurtEvent(BobHurtEvent& event)
{
    auto matrix = event.mMatrix;
    if (mSmallItems.mValue) *matrix = glm::translate(*matrix, glm::vec3(0.5f, -0.2f, -0.6f));

    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    auto heldItem = player->getSupplies()->getContainer()->getItem(player->getSupplies()->mSelectedSlot);
    bool isHoldingSword = heldItem && heldItem->mItem && heldItem->getItem()->isSword();
    if ((!ClientInstance::get()->getMouseGrabbed() && ImGui::IsMouseDown(ImGuiMouseButton_Right) && isHoldingSword || event.mDoBlockAnimation) && mAnimation.mValue != Animation::Default)
    {
        *matrix = glm::translate(*matrix, glm::vec3(0.4, 0.0, -0.15));
        *matrix = glm::translate(*matrix, glm::vec3(-0.1f, 0.15f, -0.2f));
        *matrix = glm::translate(*matrix, glm::vec3(-0.24F, 0.25f, -0.20F));
        *matrix = glm::rotate(*matrix, -1.98F, glm::vec3(0.0F, 1.0F, 0.0F));
        *matrix = glm::rotate(*matrix, 1.30F, glm::vec3(4.0F, 0.0F, 0.0F));
        *matrix = glm::rotate(*matrix, 59.9F, glm::vec3(0.0F, 1.0F, 0.0F));
        *matrix = glm::translate(*matrix, glm::vec3(0.0f, -0.1f, 0.15f));
        *matrix = glm::translate(*matrix, glm::vec3(0.08f, 0.0f, 0.0f));
        *matrix = glm::scale(*matrix, glm::vec3(1.05f, 1.05f, 1.05f));
    }


    if (mAnimation.mValue != Animation::Test) return;

    int swingTime = mSwingTime.as<int>();
    int swingProgress = mSwingDuration;
    int oldSwingProgress = mOldSwingDuration;
    float lerpedSwingProgress = MathUtils::lerp(oldSwingProgress, swingProgress, ClientInstance::get()->getMinecraftSim()->getGameSim()->mDeltaTime);
    float percent = lerpedSwingProgress / swingTime;
    spdlog::info("swingProgress: {} oldSwingProgress: {} lerpedSwingProgress: {} percent: {}", swingProgress, oldSwingProgress, lerpedSwingProgress, percent);

    // TODO: Take transforms from Java Edition that swing and apply them here (they should work fine for bedrock?)
};