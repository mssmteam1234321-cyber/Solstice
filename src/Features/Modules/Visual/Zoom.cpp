//
// Created by alteik on 15/10/2024.
//

#include "Zoom.hpp"
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Options.hpp>

void Zoom::onEnable()
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if(!player) return;

    mPastFov = ClientInstance::get()->getOptions()->mGfxFieldOfView->mValue;
    ClientInstance::get()->getOptions()->mGfxFieldOfView->mValue = 30;
}

void Zoom::onDisable()
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if(!player) return;

    ClientInstance::get()->getOptions()->mGfxFieldOfView->mValue = mPastFov;
}
