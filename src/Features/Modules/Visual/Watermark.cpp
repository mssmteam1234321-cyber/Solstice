//
// Created by vastrakai on 6/29/2024.
//

#include "Watermark.hpp"

#include <imgui.h>
#include <Features/Events/RenderEvent.hpp>
#include <Utils/FontHelper.hpp>
#include <Utils/MiscUtils/ColorUtils.hpp>
#include <Utils/MiscUtils/ImRenderUtils.hpp>

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
    ImGui::PushFont(FontHelper::Fonts["mojangles_large"]);
    auto renderPosition = ImVec2(20.f, 20.f);
    static std::string watermarkText = "solstice";
    static
    // Draw shadow
    float size = 50;
    // prevent the font from having any smoothing
    // Draw the text
    //ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, renderPosition, IM_COL32(255, 255, 255, 255), watermarkText.c_str());
    // for each character in the string
    /*
     *int i = 0;
    for (char c : str) {
        ImVec2 charSize = ImGui::GetFont()->CalcTextSizeA(size, FLT_MAX, 0.0f, std::string(1, c).c_str());

        // Add Glow
        DrawList->AddShadowCircle(ImVec2(pos.x + (charSize.x / 2), pos.y + (charSize.y / 2)),
                                                        size / 3, color[i], 100, ImVec2(0.f, 0.f), 0, 12);

        pos.x += charSize.x;
        i++;
    }*/

    for (int i = 0; i < watermarkText.length(); i++)
    {
        // get the character
        char c = watermarkText[i];
        // get the width of the character
        ImVec2 charSize = ImGui::GetFont()->CalcTextSizeA(size, FLT_MAX, 0.0f, &c, &c + 1);
        float charWidth = charSize.x;

        ImColor color = ColorUtils::getThemedColor(i * 100);

        if (mGlow.mValue)
            ImGui::GetBackgroundDrawList()->AddShadowCircle(ImVec2(renderPosition.x + (charSize.x / 2), renderPosition.y + (charSize.y / 2)),
                                                            size / 3, color, 100, ImVec2(0.f, 0.f), 0, 12);
        // draw a shadow
        ImColor shadowColor = ImColor(color.Value.x * 0.03f, color.Value.y * 0.03f, color.Value.z * 0.03f, 0.9f);
        ImVec2 shadowPos = renderPosition;
        shadowPos.x += 1.f;
        shadowPos.y += 1.f;
        ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, shadowPos, shadowColor, &c, &c + 1);

        // draw the character
        ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, renderPosition, color, &c, &c + 1);
        // move the render position to the right
        renderPosition.x += charWidth;
    }

    ImGui::PopFont();
}
