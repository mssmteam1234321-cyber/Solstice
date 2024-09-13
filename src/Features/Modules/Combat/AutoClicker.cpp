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
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &AutoClicker::onBaseTickEvent>(this);

    randomizeCPS();
}

void AutoClicker::onDisable() {
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &AutoClicker::onBaseTickEvent>(this);
}

void AutoClicker::onBaseTickEvent(class BaseTickEvent &event) {
    auto ci = ClientInstance::get();

    if (ci->getScreenName() != "hud_screen") return;

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