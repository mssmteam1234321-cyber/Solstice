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

struct TempRenderInfo
{
    std::string moduleName;
    ImVec2 start;
    ImVec2 end;
    ImColor color;
    Module* mod;
};

struct TempLineRenderInfo
{
    enum class Type
    {
        Top,
        Bottom,
        Left,
        Right
    };

    std::string moduleName;
    ImVec2 start;
    ImVec2 end;
    ImColor color;
    Module* mod;
};



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

    std::ranges::sort(module, [this](const std::shared_ptr<Module>& a, const std::shared_ptr<Module>& b)
    {
        std::string aName = a->getName();
        std::string bName = b->getName();
        if (!a->getSettingDisplayText().empty() && mRenderMode.mValue) aName += " " + a->getSettingDisplayText();
        if (!b->getSettingDisplayText().empty() && mRenderMode.mValue) bName += " " + b->getSettingDisplayText();
        return ImGui::GetFont()->CalcTextSizeA(fontSize, FLT_MAX, 0, aName.c_str()).x > ImGui::GetFont()->CalcTextSizeA(fontSize, FLT_MAX, 0, bName.c_str()).x;
    });

    // Render in top-right corner
    ImVec2 pos = ImVec2(ImGui::GetIO().DisplaySize.x - 10, 10);

    ImVec2 displayRes = ImGui::GetIO().DisplaySize;


    for (auto& mod : module)
    {
        if (!mod->mVisibleInArrayList.mValue) continue;
        if (mVisibility.mValue == static_cast<int>(ModuleVisibility::Bound) && mod->mKey == 0) continue;
        mod->mArrayListAnim = MathUtils::lerp(mod->mArrayListAnim, mod->mEnabled ? 1.f : 0.f, ImGui::GetIO().DeltaTime * 12.f);
        mod->mArrayListAnim = MathUtils::clamp(mod->mArrayListAnim, 0.f, 1.f);
        if (mod->mArrayListAnim < 0.01f) continue;

        ImColor color = ColorUtils::getThemedColor(pos.y * 2);

        std::string name = mod->getName();
        std::string settingDisplay = mod->getSettingDisplayText();
        if (!mRenderMode.mValue) settingDisplay = "";
        if (!settingDisplay.empty()) settingDisplay = " " + settingDisplay;
        ImVec2 textSize = ImGui::GetFont()->CalcTextSizeA(fontSize, FLT_MAX, 0, name.c_str());

        ImVec2 textPos = ImVec2(pos.x, pos.y);

        if (mLine.mValue == static_cast<int>(Line::Bar))
        {
            textPos.x -= 7;
        }

        ImVec2 displaySize = {0, 0};
        if (!settingDisplay.empty())
        {
            displaySize = ImGui::GetFont()->CalcTextSizeA(fontSize, FLT_MAX, 0, settingDisplay.c_str());
        }
        float endPos = textPos.x - textSize.x - displaySize.x;

        textPos.x = MathUtils::lerp(displayRes.x, endPos, mod->mArrayListAnim);
        ImVec2 mousePos = ImGui::GetIO().MousePos;
        if (glow) drawList->AddShadowRect(ImVec2(textPos.x, textPos.y), ImVec2(textPos.x + ImGui::GetFont()->CalcTextSizeA(fontSize, FLT_MAX, 0, (name + settingDisplay).c_str()).x, textPos.y + textSize.y), ImColor(color.Value.x, color.Value.y, color.Value.z, 1.f * mod->mArrayListAnim), glowStrength * mod->mArrayListAnim, ImVec2(0.f, 0.f), 0, 12);

        pos.y += (textSize.y * mod->mArrayListAnim);

    }

    pos = ImVec2(ImGui::GetIO().DisplaySize.x - 10, 10); // Reset position

    std::vector<TempRenderInfo> backgroundRects;

    for (auto& mod : module)
    {
        if (mVisibility.mValue == static_cast<int>(ModuleVisibility::Bound) && mod->mKey == 0) continue;
        if (mod->mArrayListAnim < 0.01f) continue;

        ImColor color = ColorUtils::getThemedColor(pos.y * 2);

        std::string name = mod->getName();
        std::string settingDisplay = mod->getSettingDisplayText();
        if (!mRenderMode.mValue) settingDisplay = "";
        if (!settingDisplay.empty()) settingDisplay = " " + settingDisplay;
        ImVec2 textSize = ImGui::GetFont()->CalcTextSizeA(fontSize, FLT_MAX, 0, name.c_str());

        ImVec2 textPos = ImVec2(pos.x, pos.y);

        bool addedPadding = false;
        if (mLine.mValue == static_cast<int>(Line::Bar))
        {
            textPos.x -= 7;
            addedPadding = true;
        }


        ImVec2 displaySize = {0, 0};
        if (!settingDisplay.empty())
        {
            displaySize = ImGui::GetFont()->CalcTextSizeA(fontSize, FLT_MAX, 0, settingDisplay.c_str());
        }
        float endPos = textPos.x - textSize.x - displaySize.x;

        textPos.x = MathUtils::lerp(displayRes.x, endPos, mod->mArrayListAnim);
        ImVec2 mousePos = ImGui::GetIO().MousePos;
        bool isHovered = mousePos.x >= textPos.x && mousePos.x <= textPos.x + textSize.x + displaySize.x && mousePos.y >= textPos.y && mousePos.y <= textPos.y + textSize.y;
        ImVec4 rect = {textPos.x, textPos.y, textPos.x + textSize.x + displaySize.x, textPos.y + textSize.y};


        backgroundRects.push_back({name, ImVec2(rect.x + (addedPadding ? 7.f : 0.f), rect.y), ImVec2(rect.z + (addedPadding ? 7.f : 0.f), rect.w), color, mod.get()});

        if (mBackground.mValue == static_cast<int>(Background::Opacity))
        {
            drawList->AddRectFilled(ImVec2(rect.x, rect.y), ImVec2(rect.z + (addedPadding ? 7.f : 0.f), rect.w), ImColor(color.Value.x * 0.03f, color.Value.y * 0.03f, color.Value.z * 0.03f, 0.7f), 0.0f);

        }

        ImRenderUtils::drawShadowText(drawList, name, textPos, color, fontSize);

        if (!settingDisplay.empty())
        {
            textPos.x += textSize.x;
            ImRenderUtils::drawShadowText(drawList, settingDisplay, textPos, ImColor(0.9f, 0.9f, 0.9f, 1.f), fontSize);
        }

        // Add an invisible button to toggle the module
        if (isHovered)
        {
            drawList->AddRectFilled(ImVec2(rect.x, rect.y), ImVec2(rect.z, rect.w), ImColor(1.f, 1.f, 1.f, 0.1f));
            if (ImGui::IsMouseClicked(0))
            {
                mod->toggle();
            }
        }

        pos.y += (textSize.y * mod->mArrayListAnim);
    }

    if (mLine.mValue == static_cast<int>(Line::None))
    {
        ImGui::PopFont();
        return;
    }

    std::vector<TempLineRenderInfo> lines = {};
    int bgi = 0;
    // Mode bar: Renders a bar on the right of the bgr
    // Mode outline: Renders an outline around each bgr as if they were all one big rect
    for (auto& [name, start, end, color, mod] : backgroundRects)
    {
        start.x -= 2.f;
        end.x -= 2.f;
        // Get all the bgs after this index
        std::vector<TempRenderInfo> bgsAfter = {};
        for (int i = bgi + 1; i < backgroundRects.size(); i++)
        {
            bgsAfter.push_back({name, backgroundRects[i].start, backgroundRects[i].end, backgroundRects[i].color, backgroundRects[i].mod});
        }

        auto next = TempRenderInfo();

        if (!bgsAfter.empty())
        {
            std::ranges::sort(bgsAfter, [](const TempRenderInfo& a, const TempRenderInfo& b)
            {
                return a.start.x < b.start.x;
            });

            next = bgsAfter.front();
        }

        bool hasNext = !next.moduleName.empty();

        if (mLine.mValue == static_cast<int>(Line::Outline))
        {
            // Side: Right
            lines.push_back({name, ImVec2(end.x + 2, start.y), ImVec2(end.x + 2, end.y), color, mod});
            // Side: Left
            if (hasNext && next.start.x >= start.x || !hasNext)
            {
                lines.push_back({name, ImVec2(start.x, start.y), ImVec2(start.x, end.y), color, mod});
            }
            else if (hasNext)
            {
                lines.push_back({name, ImVec2(next.start.x, start.y), ImVec2(next.start.x, start.y), color, mod});
            }
            // Side: Top
            if (bgi == 0)
            {
                lines.push_back({name, ImVec2(start.x, start.y), ImVec2(end.x + 2, start.y), color, mod});
            }
            // Side: Bottom
            if (!hasNext)
            {
                lines.push_back({name, ImVec2(start.x, end.y), ImVec2(end.x + 2, end.y), color, mod});
            } else if (next.start.x >= start.x)
            {
                lines.push_back({name, ImVec2(start.x, end.y), ImVec2(next.start.x, end.y), color, mod});
            }

        }
        else if (mLine.mValue == static_cast<int>(Line::Bar))
        {
            lines.push_back({name, ImVec2(end.x, start.y), ImVec2(end.x, end.y), color, mod});
        }

        bgi++;
    }



    for (auto& line : lines)
    {
        drawList->AddLine(ImVec2(line.start.x, line.start.y), ImVec2(line.end.x, line.end.y), line.color, mLine.mValue == static_cast<int>(Line::Outline) ? 2.f : 4.f);
    }

    ImGui::PopFont();
}