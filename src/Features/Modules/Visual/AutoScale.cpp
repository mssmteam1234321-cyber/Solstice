//
// Created by vastrakai on 6/30/2024.
//

#include "AutoScale.hpp"

#include <Features/FeatureManager.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Rendering/GuiData.hpp>

void AutoScale::onEnable()
{
    // Store the old values
    auto guiData = ClientInstance::get()->getGuiData();
    mOldGuiScale = guiData->guiScale;
    mOldScaleMultiplier = guiData->scalingMultiplier;
    mOldScaledResolution = guiData->resolutionScaled;

    // Set the new values
    guiData->guiScale = mScaleSetting.mValue;
    guiData->scalingMultiplier = 1 / mScaleSetting.mValue;
    guiData->resolutionScaled = glm::vec2(guiData->resolution.x * guiData->scalingMultiplier, guiData->resolution.y * guiData->scalingMultiplier);

    gFeatureManager->mDispatcher->listen<RenderEvent, &AutoScale::onRenderEvent>(this);
}

void AutoScale::onDisable()
{
    gFeatureManager->mDispatcher->deafen<RenderEvent, &AutoScale::onRenderEvent>(this);

    // Restore the old values
    auto guiData = ClientInstance::get()->getGuiData();
    guiData->guiScale = mOldGuiScale;
    guiData->scalingMultiplier = mOldScaleMultiplier;
    guiData->resolutionScaled = mOldScaledResolution;
}

void AutoScale::onRenderEvent(RenderEvent& event)
{
    float scale = mScaleSetting.mValue;
    auto guiData = ClientInstance::get()->getGuiData();
    guiData->guiScale = scale;
    guiData->scalingMultiplier = 1 / scale;
    guiData->resolutionScaled = glm::vec2(guiData->resolution.x * guiData->scalingMultiplier, guiData->resolution.y * guiData->scalingMultiplier);
}
