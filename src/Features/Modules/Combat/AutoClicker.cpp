//
// Created by alteik on 04/09/2024.
//

#include "AutoClicker.hpp"
#include <Features/Events/BaseTickEvent.hpp>
#include <Utils/GameUtils/ActorUtils.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Actor/GameMode.hpp>
#include <SDK/Minecraft/World/Level.hpp>
#include <SDK/Minecraft/World/HitResult.hpp>
#include <Features/Modules/Misc/AntiBot.hpp>
#include <Hook/Hooks/MiscHooks/MouseHook.hpp>
#include <SDK/Minecraft/Actor/Components/RuntimeIDComponent.hpp>

void AutoClicker::onEnable() {
    gFeatureManager->mDispatcher->listen<RenderEvent, &AutoClicker::onRenderEvent>(this);

    randomizeCPS();
}

void AutoClicker::onDisable() {
    gFeatureManager->mDispatcher->deafen<RenderEvent, &AutoClicker::onRenderEvent>(this);
}

void AutoClicker::onRenderEvent(RenderEvent& event)
{
    auto ci = ClientInstance::get();

    if (!ci->getLocalPlayer() || ci->getScreenName() != "hud_screen") return;

    auto hitres = ci->getLocalPlayer()->getLevel()->getHitResult();
    if (mAllowBlockBreaking.mValue && hitres && hitres->mType == HitType::BLOCK)
        return;

    if (mRandomCPSMin.as<int>() > mRandomCPSMax.as<int>())
        mRandomCPSMin.mValue = mRandomCPSMax.mValue;

    int button = mClickMode.as<int>();

    if (mHold.mValue && !ImGui::IsMouseDown(button)) return;

    static uint64_t lastAction = 0;

    if (NOW - lastAction < 1000 / mCurrentCPS) return;

    lastAction = NOW;

    MouseHook::simulateMouseInput(button + 1, 1, 0, 0, 0, 0);
    MouseHook::simulateMouseInput(button + 1, 0, 0, 0, 0, 0);

    randomizeCPS();
}
