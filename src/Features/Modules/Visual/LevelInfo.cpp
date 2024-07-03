//
// Created by vastrakai on 7/2/2024.
//

#include "LevelInfo.hpp"

#include <Features/FeatureManager.hpp>
#include <Utils/FontHelper.hpp>
#include <Utils/MiscUtils/ImRenderUtils.hpp>

void LevelInfo::onEnable()
{
    gFeatureManager->mDispatcher->listen<RenderEvent, &LevelInfo::onRenderEvent>(this);
}

void LevelInfo::onDisable()
{
    gFeatureManager->mDispatcher->deafen<RenderEvent, &LevelInfo::onRenderEvent>(this);
}

void LevelInfo::onRenderEvent(RenderEvent& event)
{
    ImGui::PushFont(FontHelper::Fonts["mojangles_large"]);

    ImVec2 pos = { 2, ImGui::GetIO().DisplaySize.y };
    std::string text = "FPS: " + std::to_string((int)ImGui::GetIO().Framerate);
    float fontSize = 20.f;
    ImVec2 textSize = ImGui::GetFont()->CalcTextSizeA(fontSize, FLT_MAX, 0.f, text.c_str());
    pos.y -= textSize.y + 2.f;
    ImRenderUtils::drawShadowText(ImGui::GetBackgroundDrawList(), text, pos, ImColor(1.f, 1.f, 1.f, 1.f), fontSize);

    ImGui::PopFont();

}
