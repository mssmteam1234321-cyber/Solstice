//
// Created by vastrakai on 6/29/2024.
//

#include "ToggleSounds.hpp"

#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/MinecraftGame.hpp>
#include <Utils/MiscUtils/SoundUtils.hpp>

void ToggleSounds::onEnable()
{
    gFeatureManager->mDispatcher->listen<ModuleStateChangeEvent, &ToggleSounds::onModuleStateChange>(this);
}

void ToggleSounds::onDisable()
{
    gFeatureManager->mDispatcher->deafen<ModuleStateChangeEvent, &ToggleSounds::onModuleStateChange>(this);
}

void ToggleSounds::onModuleStateChange(ModuleStateChangeEvent& event)
{
    if (event.isCancelled()) return;

    if (mSound.mValue == Lever) ClientInstance::get()->getMinecraftGame()->playUi("random.lever_click", 1, event.mEnabled ? 0.6f : 0.5f);
    else if (mSound.mValue == Smooth) SoundUtils::playSoundFromEmbeddedResource(event.mEnabled ? "smooth_on.wav" : "smooth_off.wav", 1);
}
