//
// Created by vastrakai on 7/2/2024.
//

#include "Arraylist.hpp"

#include <set>
#include <Features/FeatureManager.hpp>
#include <Utils/FontHelper.hpp>
#include <Utils/MiscUtils/ColorUtils.hpp>
#include <Utils/MiscUtils/EasingUtil.hpp>
#include <Utils/MiscUtils/ImRenderUtils.hpp>
#include <Utils/MiscUtils/MathUtils.hpp>

void Arraylist::onEnable()
{
    gFeatureManager->mDispatcher->listen<RenderEvent, &Arraylist::onRenderEvent>(this);
}

void Arraylist::onDisable()
{
    gFeatureManager->mDispatcher->deafen<RenderEvent, &Arraylist::onRenderEvent>(this);

    for (auto& mod : gFeatureManager->mModuleManager->getModules())
    {
        mod->mArrayListAnim = 0.f;
    }
}



void drawShadowTextW(ImDrawList* drawList, const std::string& text, ImVec2 pos, ImColor color, float size, bool shadow = true)
{
    ImVec2 shadowPos = pos;
    shadowPos.x += 1.f;
    shadowPos.y += 1.f;
    ImVec2 textPos = pos;
    //if (glow && !secondCall) drawList->AddShadowCircle(ImVec2(textPos.x, textPos.y + (size / 2)), size / 3, ImColor(color.Value.x, color.Value.y, color.Value.z, 1.f), glowStrength, ImVec2(0.f, 0.f), 0, 12);
    // draw a glow rect instead
    for (int i = 0; i < text.length(); i++)
    {
        char c = text[i];
        //if (glow) drawList->AddShadowCircle(ImVec2(textPos.x + (size / 2), textPos.y + (size / 2)), size / 3, ImColor(color.Value.x, color.Value.y, color.Value.z, 1.f), glowStrength, ImVec2(0.f, 0.f), 0, 12);
        if (shadow) drawList->AddText(ImGui::GetFont(), size, shadowPos, ImColor(color.Value.x * 0.03f, color.Value.y * 0.03f, color.Value.z * 0.03f, 0.9f), &c, &c + 1);
        drawList->AddText(ImGui::GetFont(), size, textPos, color, &c, &c + 1);
        textPos.x += ImGui::GetFont()->CalcTextSizeA(size, FLT_MAX, 0, &c, &c + 1).x;
        shadowPos.x += ImGui::GetFont()->CalcTextSizeA(size, FLT_MAX, 0, &c, &c + 1).x;
    }
}

void Arraylist::onRenderEvent(RenderEvent& event)
{
    bool glow = mGlow.mValue;
    float glowStrength = mGlowStrength.mValue * 100.f;

    // Render the array list here
    static std::vector<std::shared_ptr<Module>> module;

    auto drawList = ImGui::GetBackgroundDrawList();
    ImGui::PushFont(FontHelper::Fonts["mojangles_large"]);

    if (module.empty())
    {
        module = gFeatureManager->mModuleManager->getModules();
    }

    static float fontSize = 25.f;

    std::ranges::sort(module, [](const std::shared_ptr<Module>& a, const std::shared_ptr<Module>& b)
    {
        std::string aName = a->getName();
        std::string bName = b->getName();
        if (!a->getSettingDisplayText().empty()) aName += " " + a->getSettingDisplayText();
        if (!b->getSettingDisplayText().empty()) bName += " " + b->getSettingDisplayText();
        return ImGui::GetFont()->CalcTextSizeA(fontSize, FLT_MAX, 0, aName.c_str()).x > ImGui::GetFont()->CalcTextSizeA(fontSize, FLT_MAX, 0, bName.c_str()).x;
    });

    // Render in top-right corner
    ImVec2 pos = ImVec2(ImGui::GetIO().DisplaySize.x - 10, 10);

    ImVec2 displayRes = ImGui::GetIO().DisplaySize;

    int i = 0;
    for (auto& mod : module)
    {
        if (!mod->mVisibleInArrayList.mValue) continue;
        mod->mArrayListAnim = MathUtils::lerp(mod->mArrayListAnim, mod->mEnabled ? 1.f : 0.f, ImGui::GetIO().DeltaTime * 12.f);
        mod->mArrayListAnim = MathUtils::clamp(mod->mArrayListAnim, 0.f, 1.f);
        if (mod->mArrayListAnim < 0.01f) continue;
        ImColor color = ColorUtils::getThemedColor(i * 100);

        std::string name = mod->getName();
        std::string settingDisplay = mod->getSettingDisplayText();
        if (!settingDisplay.empty()) settingDisplay = " " + settingDisplay;
        ImVec2 textSize = ImGui::GetFont()->CalcTextSizeA(fontSize, FLT_MAX, 0, name.c_str());

        ImVec2 textPos = ImVec2(pos.x, pos.y);

        ImVec2 displaySize = {0, 0};
        if (!settingDisplay.empty())
        {
            displaySize = ImGui::GetFont()->CalcTextSizeA(fontSize, FLT_MAX, 0, settingDisplay.c_str());
        }
        float endPos = textPos.x - textSize.x - displaySize.x;

        textPos.x = MathUtils::lerp(displayRes.x, endPos, mod->mArrayListAnim);
        if (glow) drawList->AddShadowRect(ImVec2(textPos.x, textPos.y), ImVec2(textPos.x + ImGui::GetFont()->CalcTextSizeA(fontSize, FLT_MAX, 0, (name + settingDisplay).c_str()).x, textPos.y + textSize.y), ImColor(color.Value.x, color.Value.y, color.Value.z, 1.f * mod->mArrayListAnim), glowStrength * mod->mArrayListAnim, ImVec2(0.f, 0.f), 0, 12);
        ImRenderUtils::drawShadowText(drawList, name, textPos, color, fontSize);

        if (!settingDisplay.empty())
        {
            textPos.x += textSize.x;
            ImRenderUtils::drawShadowText(drawList, settingDisplay, textPos, ImColor(0.9f, 0.9f, 0.9f, 1.f), fontSize);
        }

        pos.y += (textSize.y * mod->mArrayListAnim);

        i++;
    }

    ImGui::PopFont();
}