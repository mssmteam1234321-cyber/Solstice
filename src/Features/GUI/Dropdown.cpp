//
// Created by vastrakai on 7/1/2024.
//

#include "Dropdown.hpp"
#include <Features/Modules/ModuleCategory.hpp>
#include <Features/Modules/Visual/ClickGui.hpp>
#include <Utils/FontHelper.hpp>
#include <Utils/MiscUtils/ImRenderUtils.hpp>
#include <Utils/MiscUtils/MathUtils.hpp>
#include <Features/Modules/Setting.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Rendering/GuiData.hpp>
#include <Utils/Keyboard.hpp>

ImVec4 DropdownGui::scaleToPoint(const ImVec4& _this, const ImVec4& point, float amount)
{
    return {point.x + (_this.x - point.x) * amount, point.y + (_this.y - point.y) * amount,
        point.z + (_this.z - point.z) * amount, point.w + (_this.w - point.w) * amount };
}

bool DropdownGui::isMouseOver(const ImVec4& rect)
{
    ImVec2 mousePos = ImGui::GetIO().MousePos;
    return mousePos.x >= rect.x && mousePos.y >= rect.y && mousePos.x < rect.z && mousePos.y < rect.w;
}

ImVec4 DropdownGui::getCenter(ImVec4& vec)
{
    float centerX = (vec.x + vec.z) / 2.0f;
    float centerY = (vec.y + vec.w) / 2.0f;
    return { centerX, centerY, centerX, centerY };
}

void DropdownGui::render(float animation, float inScale, int& scrollDirection, char* h, bool blur)
{
    ImGui::PushFont(FontHelper::Fonts["mojangles"]);
    ImVec2 screenSize = ImGui::GetIO().DisplaySize;
    float deltaTime = ImGui::GetIO().DeltaTime;
    ImVec2 cursorPos = ImGui::GetCursorPos();

    auto drawList = ImGui::GetForegroundDrawList();

    if (blur)
    {
        /*ImRenderUtils::fillRectangle(
                ImVec4(0, 0, screen.x, screen.y),
                ImColor(0, 0, 0), animation * 0.38f);*/
        drawList->AddRectFilled(ImVec2(0, 0), ImVec2(screenSize.x, screenSize.y), IM_COL32(0, 0, 0, 255 * animation * 0.38f));
        ImRenderUtils::addBlur(ImVec4(0.f, 0.f, screenSize.x, screenSize.y),
                               animation * 9, 0);
    }

    static std::vector<std::string> categories = ModuleCategoryNames;
    std::unordered_map<std::string, std::shared_ptr<Module>> modCategoryMap = gFeatureManager->mModuleManager->
        getModuleCategoryMap();

    static std::unordered_map<std::string, int> categoryIndexes = ModuleCategoryMap;

    static auto* clickGui = gFeatureManager->mModuleManager->getModule<ClickGui>();
    bool isEnabled = clickGui->mEnabled;

    int categoriesSize = categories.size();
    float categoryHeight = 30.f;
    float categoryWidth = 200.f;
    float categorySpacing = 5.f;

    float totalHeight = categoriesSize * categoryHeight + (categoriesSize - 1) * categorySpacing;


    ImGui::PopFont();
}
