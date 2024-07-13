//
// Created by vastrakai on 7/12/2024.
//

#include "MotionBlur.hpp"

#include <Features/FeatureManager.hpp>
#include <Utils/MiscUtils/D2D.hpp>

void MotionBlur::onEnable()
{
    gFeatureManager->mDispatcher->listen<RenderEvent, &MotionBlur::onRenderEvent, nes::event_priority::LAST>(this);
}

void MotionBlur::onDisable()
{
    gFeatureManager->mDispatcher->deafen<RenderEvent, &MotionBlur::onRenderEvent>(this);
}

void MotionBlur::onRenderEvent(RenderEvent& event)
{
    D2D::addGhostFrame(ImGui::GetBackgroundDrawList(), mMaxFrames.as<int>(), mIntensity.as<float>());
}
