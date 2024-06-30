//
// Created by vastrakai on 6/29/2024.
//

#include "ClickGui.hpp"

#include <imgui.h>
#include <Utils/MiscUtils/ImRenderUtils.hpp>
#include <Utils/MiscUtils/MathUtils.hpp>

void ClickGui::onEnable()
{

}

void ClickGui::onDisable()
{
}

void ClickGui::onRenderEvent(RenderEvent& event)
{
    static float animationPercent = 0.0f;
    float targetPercent = mEnabled ? 1.0f : 0.0f;
    animationPercent = MathUtils::animate(animationPercent, targetPercent, ImGui::GetIO().DeltaTime * 10.0f);

    auto bg = ImGui::GetForegroundDrawList();
    ImVec2 windowSize = ImGui::GetIO().DisplaySize;

    bg->AddRectFilled(ImVec2(0, 0), windowSize, ImColor(0.f, 0.f, 0.f, 0.6f * animationPercent));

    ImRenderUtils::addBlur(ImVec4(0, 0, windowSize.x, windowSize.y), 9.f * animationPercent, 0.f);
}
