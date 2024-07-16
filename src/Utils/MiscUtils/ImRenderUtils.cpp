//
// Created by vastrakai on 6/29/2024.
//

#include "ImRenderUtils.hpp"

#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Rendering/GuiData.hpp>
#include <Utils/ProcUtils.hpp>

#include "D2D.hpp"

void ImRenderUtils::addBlur(const ImVec4& pos, float strength, float radius)
{
    if (!ImGui::GetCurrentContext())
        return;

    D2D::addBlur(ImGui::GetBackgroundDrawList(), strength, pos, radius);
}

void ImRenderUtils::drawText(ImVec2 pos, const std::string& textStr, const ImColor& color, float textSize, float alpha,
    bool shadow, int index, ImDrawList* d)
{
    if (!ImGui::GetCurrentContext())
        return;
    ImFont* font = ImGui::GetFont();



    ImVec2 textPos = ImVec2(pos.x, pos.y);
    constexpr ImVec2 shadowOffset = ImVec2(1.f, 1.f);
    constexpr ImVec2 shadowOffsetMinecraft = ImVec2(1.45f, 1.45f);

    if (shadow)
    {
        ImVec2 added = textPos;
        added.x = added.x + shadowOffset.x;
        added.y = added.y + shadowOffset.y;
        /*if (font == ImGui::GetIO().Fonts->Fonts[4]) {
				d->AddText(font, (textSize * 18), textPos + shadowOffsetMinecraft, ImColor(color.Value.x * 0.2f, color.Value.y * 0.2f, color.Value.z * 0.2f, alpha * 0.7f), textStr->c_str());
			}
			else {*/
        d->AddText(font, (textSize * 18), added, ImColor(color.Value.x * 0.03f, color.Value.y * 0.03f, color.Value.z * 0.03f, alpha * 0.9f), textStr.c_str());
        //}
    }

    d->AddText(font, (textSize * 18), textPos, ImColor(color.Value.x, color.Value.y, color.Value.z, alpha), textStr.c_str());
}

void ImRenderUtils::drawShadowText(ImDrawList* drawList, const std::string& text, ImVec2 pos, ImColor color, float fontSize)
{
    ImVec2 shadowPos = pos;
    shadowPos.x += 2.f;
    shadowPos.y += 2.f;
    drawList->AddText(ImGui::GetFont(), fontSize, shadowPos, ImColor(color.Value.x * 0.25f, color.Value.y * 0.25f, color.Value.z * 0.25f, 0.9f), text.c_str());
    drawList->AddText(ImGui::GetFont(), fontSize, pos, color, text.c_str());
}

void ImRenderUtils::fillRectangle(ImVec4 pos, const ImColor& color, float alpha, float radius, ImDrawList* list, ImDrawFlags flags)
{
    if (!ImGui::GetCurrentContext())
        return;

    list->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.z, pos.w), ImColor(color.Value.x, color.Value.y, color.Value.z, alpha), radius, flags);
}

void ImRenderUtils::fillShadowRectangle(ImVec4 pos, const ImColor& color, float alpha, float thickness,
    ImDrawFlags flags, float radius, ImDrawList* list)
{
    if (!ImGui::GetCurrentContext())
        return;

    ImVec2 offset = ImVec2(0, 0);
    list->AddShadowRect(ImVec2(pos.x, pos.y), ImVec2(pos.z, pos.w), ImColor(color.Value.x, color.Value.y, color.Value.z, alpha), thickness, offset, flags, radius);
}

void ImRenderUtils::fillShadowCircle(ImVec2 pos, float radius, const ImColor& color, float alpha, float thickness,
    ImDrawFlags flags, float segments)
{
    if (!ImGui::GetCurrentContext())
        return;

    ImDrawList* list = ImGui::GetBackgroundDrawList();
    ImVec2 offset = ImVec2(0, 0);
    list->AddShadowCircle(ImVec2(pos.x, pos.y), radius, ImColor(color.Value.x, color.Value.y, color.Value.z, alpha), thickness, offset, flags, radius);
}

void ImRenderUtils::drawRoundRect(ImVec4 pos, const ImDrawFlags& flags, float radius, const ImColor& color, float alpha,
    float lineWidth)
{
    if (!ImGui::GetCurrentContext())
        return;
    const auto d = ImGui::GetBackgroundDrawList();
    d->AddRect(ImVec2(pos.x, pos.y), ImVec2(pos.z, pos.w), ImColor(color.Value.x, color.Value.y, color.Value.z, alpha), radius, flags, lineWidth);
}

void ImRenderUtils::fillGradientOpaqueRectangle(ImVec4 pos, const ImColor& firstColor, const ImColor& secondColor,
    float firstAlpha, float secondAlpha)
{
    if (!ImGui::GetCurrentContext())
        return;

    ImDrawList* list = ImGui::GetBackgroundDrawList();

    ImVec2 topLeft = ImVec2(pos.x, pos.y);
    ImVec2 bottomRight = ImVec2(pos.z, pos.w);

    list->AddRectFilledMultiColor(topLeft, bottomRight,
                                  ImColor(firstColor.Value.x, firstColor.Value.y, firstColor.Value.z, secondAlpha),
                                  ImColor(secondColor.Value.x, secondColor.Value.y, secondColor.Value.z, secondAlpha),
                                  ImColor(secondColor.Value.x, secondColor.Value.y, secondColor.Value.z, firstAlpha),
                                  ImColor(firstColor.Value.x, firstColor.Value.y, firstColor.Value.z, firstAlpha));
}

void ImRenderUtils::fillRoundedGradientRectangle(ImVec4 pos, const ImColor& firstColor, const ImColor& secondColor,
    float radius, float firstAlpha, float secondAlpha)
{
    if (!ImGui::GetCurrentContext())
        return;

    ImDrawList* list = ImGui::GetBackgroundDrawList();

    ImVec2 topLeft = ImVec2(pos.x, pos.y);
    ImVec2 bottomRight = ImVec2(pos.z, pos.w);

    list->AddRectFilledMultiColor(topLeft, bottomRight,
                                  ImColor(firstColor.Value.x, firstColor.Value.y, firstColor.Value.z, secondAlpha),
                                  ImColor(secondColor.Value.x, secondColor.Value.y, secondColor.Value.z, secondAlpha),
                                  ImColor(secondColor.Value.x, secondColor.Value.y, secondColor.Value.z, firstAlpha),
                                  ImColor(firstColor.Value.x, firstColor.Value.y, firstColor.Value.z, firstAlpha));
}

void ImRenderUtils::fillCircle(ImVec2 center, float radius, const ImColor& color, float alpha, int segments)
{
    if (!ImGui::GetCurrentContext())
        return;

    ImDrawList* list = ImGui::GetBackgroundDrawList();
    list->AddCircleFilled(ImVec2(center.x , center.y), radius, ImColor(color.Value.x, color.Value.y, color.Value.z, alpha), segments);
}

ImVec2 ImRenderUtils::getScreenSize()
{
    glm::vec2 resolution = ClientInstance::get()->getGuiData()->resolution;
    return ImVec2(resolution.x, resolution.y);
}

bool ImRenderUtils::isFullScreen()
{
    RECT desktop;

    GetWindowRect(ProcUtils::getMinecraftWindow(), &desktop);
    if (desktop.top != 0 || desktop.left != 0)
        return false;
    return true;
}

float ImRenderUtils::getTextHeightStr(std::string* textStr, float textSize)
{
    return ImGui::GetFont()->CalcTextSizeA(textSize * 18, FLT_MAX, -1, textStr->c_str()).y;
}

float ImRenderUtils::getTextWidth(std::string* textStr, float textSize)
{
    return ImGui::GetFont()->CalcTextSizeA(textSize * 18, FLT_MAX, -1, textStr->c_str()).x;
}

float ImRenderUtils::getTextHeight(float textSize)
{
    return ImGui::GetFont()->CalcTextSizeA(textSize * 18, FLT_MAX, -1, "").y;
}

float ImRenderUtils::getDeltaTime()
{
    return ImGui::GetIO().DeltaTime;
}

ImVec2 ImRenderUtils::getMousePos()
{
    return ImGui::GetIO().MousePos;
}

bool ImRenderUtils::isMouseOver(ImVec4 pos)
{
    ImVec2 mousePos = getMousePos();
    return mousePos.x >= pos.x && mousePos.y >= pos.y && mousePos.x < pos.z && mousePos.y < pos.w;
}
