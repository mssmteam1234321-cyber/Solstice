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

void drawShadowText(ImDrawList* drawList, const std::string& text, ImVec2 pos, ImColor color, float fontSize)
{
    ImVec2 shadowPos = pos;
    shadowPos.x += 1.f;
    shadowPos.y += 1.f;
    drawList->AddText(ImGui::GetFont(), fontSize, shadowPos, ImColor(color.Value.x * 0.03f, color.Value.y * 0.03f, color.Value.z * 0.03f, 0.9f), text.c_str());
    drawList->AddText(ImGui::GetFont(), fontSize, pos, color, text.c_str());
}

void drawSolsticeWatermark()
{
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
        std::string aName = a->mName;
        std::string bName = b->mName;
        if (!a->getSettingDisplay().empty()) aName += " " + a->getSettingDisplay();
        if (!b->getSettingDisplay().empty()) bName += " " + b->getSettingDisplay();
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

        std::string name = mod->mName;
        std::string settingDisplay = mod->getSettingDisplay();
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
        drawShadowText(drawList, name, textPos, color, fontSize);

        if (!settingDisplay.empty())
        {
            textPos.x += textSize.x;
            drawShadowText(drawList, settingDisplay, textPos, ImColor(0.9f, 0.9f, 0.9f, 1.f), fontSize);
        }

        pos.y += (textSize.y * mod->mArrayListAnim);

        i++;
    }

    ImGui::PopFont();
}

void Arraylist::onRenderEvent(RenderEvent& event)
{
    if (mStyle.mValue == static_cast<int>(Style::Solstice))
    {
        drawSolsticeWatermark();
    }
}