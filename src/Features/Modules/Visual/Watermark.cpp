//
// Created by vastrakai on 6/29/2024.
//

#include "Watermark.hpp"

#include <imgui.h>
#include <Features/Events/RenderEvent.hpp>

void Watermark::onEnable()
{
    gFeatureManager->mDispatcher->listen<RenderEvent, &Watermark::onRenderEvent>(this);
}

void Watermark::onDisable()
{
    gFeatureManager->mDispatcher->deafen<RenderEvent, &Watermark::onRenderEvent>(this);
}

void Watermark::onRenderEvent(RenderEvent& event)
{
    // Render the watermark here using ImGui
    auto renderPosition = ImVec2(40.f, 40.f);
    static std::string watermarkText = "Solstice";
    // Draw shadow
    float size = 50;

    //ImGui::PushFont(FontHelper::Fonts["mojangles_large"]);
    ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, renderPosition, IM_COL32(0, 0, 0, 255), watermarkText.c_str());
    ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, renderPosition, IM_COL32(255, 255, 255, 255), watermarkText.c_str());
    //ImGui::PopFont();
}
