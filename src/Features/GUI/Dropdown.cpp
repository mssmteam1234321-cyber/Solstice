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
#include <Utils/MiscUtils/ColorUtils.hpp>

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
    ImGui::PushFont(FontHelper::Fonts["mojangles_large"]);
    ImVec2 screen = ImRenderUtils::getScreenSize();
    float deltaTime = ImGui::GetIO().DeltaTime;
    auto drawList = ImGui::GetBackgroundDrawList();

    if (blur)
    {
        /*ImRenderUtils::fillRectangle(
                ImVec4(0, 0, screen.x, screen.y),
                ImColor(0, 0, 0), animation * 0.38f);*/
        drawList->AddRectFilled(ImVec2(0, 0), ImVec2(screen.x, screen.y), IM_COL32(0, 0, 0, 255 * animation * 0.38f));
        ImRenderUtils::addBlur(ImVec4(0.f, 0.f, screen.x, screen.y),
                               animation * 9, 0);
    }

    static std::vector<std::string> categories = ModuleCategoryNames;
    static std::vector<std::shared_ptr<Module>>& modules = gFeatureManager->mModuleManager->getModules();

    static auto* clickGui = gFeatureManager->mModuleManager->getModule<ClickGui>();
    bool isEnabled = clickGui->mEnabled;
    std::string tooltip = "";

    float textSize = inScale;
    //float textHeight = ImRenderUtils::getTextHeight(textSize);
    float textHeight = ImGui::GetFont()->CalcTextSizeA(textSize * 18, FLT_MAX, -1, "").y;

    int screenWidth = (int)screen.x;
    int screenHeight = 10;

    float windowWidth = 220.0f;
    float windowHeight = 190.0f;
    float yOffset = 50.0f;
    float windowX = (screenWidth - windowWidth) * 0.5f;
    float windowY = screenHeight;

    if (displayColorPicker && isEnabled)
    {
        ImGui::PushFont(FontHelper::Fonts["mojangles"]);
        ColorSetting* colorSetting = lastColorSetting;
        // Display the color picker in the bottom middle of the screen
        ImGui::SetNextWindowPos(ImVec2(screen.x / 2 - 200, screen.y / 2));
        ImGui::SetNextWindowSize(ImVec2(400, 400));

        ImGui::Begin("Color Picker", &displayColorPicker, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
        {
            ImVec4 color = colorSetting->getAsImColor().Value;
            ImGui::ColorPicker4("Color", colorSetting->mValue, ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoAlpha);
            ImGui::Button("Close");
            if (ImGui::IsItemClicked())
            {
                // Set the color setting to the new color
                colorSetting->setFromImColor(ImColor(color));
                displayColorPicker = false;
            }
        }
        ImGui::End();
        ImGui::PopFont();

        if (ImGui::IsMouseClicked(0) && !ImRenderUtils::isMouseOver(ImVec4(screen.x / 2 - 200, screen.y / 2, screen.x / 2 + 200, screen.y / 2 + 400)))
        {
            displayColorPicker = false;
        }
    }

    if (!isEnabled) displayColorPicker = false;

    if (catPositions.empty() && isEnabled)
    {
        float centerX = screen.x / 2.f;
        float xPos = centerX - (categories.size() * (catWidth + catGap) / 2);
        for (std::string& category : categories)
        {
            CategoryPosition pos;
            pos.x = xPos;
            pos.y = catGap * 2;
            xPos += catWidth + catGap;
            catPositions.push_back(pos);
        }
    }

    if (!catPositions.empty())
    {
        for (size_t i = 0; i < categories.size(); i++)
        {
            // Mod math stuff
            const float modWidth = catWidth;
            const float modHeight = catHeight;
            float moduleY = -catPositions[i].yOffset;

            // Get all the modules and populate our vector
            const auto& modsInCategory = gFeatureManager->mModuleManager->getModulesInCategory(i);

            // Calculate the catRect pos
            ImVec4 catRect = ImVec4(catPositions[i].x, catPositions[i].y,
                                                    catPositions[i].x + catWidth, catPositions[i].y + catHeight)
                .scaleToPoint(ImVec4(screen.x / 2,
                                             screen.y / 2,
                                             screen.x / 2,
                                             screen.y / 2), inScale);

            /* Calculate the height of the catWindow including the settings */
            float settingsHeight = 0;

            for (const auto& mod : modsInCategory)
            {
                std::string modLower = mod->mName;

                std::transform(modLower.begin(), modLower.end(), modLower.begin(), [](unsigned char c)
                {
                    return std::tolower(c);
                });

                for (const auto& setting : mod->mSettings)
                {
                    switch (setting->mType)
                    {
                    case SettingType::Bool:
                        {
                            settingsHeight = MathUtils::lerp(settingsHeight, settingsHeight + modHeight, mod->cAnim);
                            break;
                        }
                    case SettingType::Enum:
                        {
                            EnumSetting* enumSetting = reinterpret_cast<EnumSetting*>(setting);
                            std::vector<std::string> enumValues = enumSetting->mValues;
                            int numValues = static_cast<int>(enumValues.size());

                            settingsHeight = MathUtils::lerp(settingsHeight, settingsHeight + modHeight, mod->cAnim);
                            if (setting->enumSlide > 0.01)
                            {
                                for (int j = 0; j < numValues; j++)
                                    settingsHeight = MathUtils::lerp(settingsHeight, settingsHeight + modHeight,
                                                                setting->enumSlide);
                            }
                            break;
                        }
                    case SettingType::Number:
                        {
                            settingsHeight = MathUtils::lerp(settingsHeight, settingsHeight + modHeight, mod->cAnim);
                            break;
                        }
                    case SettingType::Color:
                        {
                            settingsHeight = MathUtils::lerp(settingsHeight, settingsHeight + modHeight, mod->cAnim);
                            break;
                        }
                    }
                }
            }

            float catWindowHeight = catHeight + modHeight * modsInCategory.size() + settingsHeight;
            ImVec4 catWindow = ImVec4(catPositions[i].x, catPositions[i].y,
                                                      catPositions[i].x + catWidth,
                                                      catPositions[i].y + moduleY + catWindowHeight)
                .scaleToPoint(ImVec4(screen.x / 2,
                                             screen.y / 2,
                                             screen.x / 2,
                                             screen.y / 2), inScale);
            ImColor rgb = ColorUtils::getThemedColor(i * 20);

            // Can we scroll?
            if (ImRenderUtils::isMouseOver(catWindow) && catPositions[i].isExtended)
            {
                if (scrollDirection > 0)
                {
                    catPositions[i].scrollEase += scrollDirection * catHeight;
                    if (catPositions[i].scrollEase > catWindowHeight - modHeight * 2)
                        catPositions[i].scrollEase = catWindowHeight - modHeight * 2;
                }
                else if (scrollDirection < 0)
                {
                    catPositions[i].scrollEase += scrollDirection * catHeight;
                    if (catPositions[i].scrollEase < 0)
                        catPositions[i].scrollEase = 0;
                }
                scrollDirection = 0;
            }

            // Lerp the category extending
            if (!catPositions[i].isExtended)
            {
                catPositions[i].scrollEase = catWindowHeight - catHeight;
                catPositions[i].wasExtended = false;
            }
            else if (!catPositions[i].wasExtended)
            {
                catPositions[i].scrollEase = 0;
                catPositions[i].wasExtended = true;
            }

            // Lerp the scrolling cuz smooth
            catPositions[i].yOffset = MathUtils::animate(catPositions[i].scrollEase, catPositions[i].yOffset,
                                                    ImRenderUtils::getDeltaTime() * 10.5);

            for (const auto& mod : modsInCategory)
            {
                std::string modLower = mod->mName;

                std::transform(modLower.begin(), modLower.end(), modLower.begin(), [](unsigned char c)
                {
                    return std::tolower(c);
                });

                ImColor rgb = ColorUtils::getThemedColor(moduleY * 2);

                // If the mod belongs to the category
                if (mod->getCategory() == categories[i])
                {
                    // Calculate the modRect pos
                    ImVec4 modRect = ImVec4(catPositions[i].x,
                                                            catPositions[i].y + catHeight + moduleY,
                                                            catPositions[i].x + modWidth,
                                                            catPositions[i].y + catHeight + moduleY + modHeight)
                        .scaleToPoint(ImVec4(screen.x / 2,
                                                     screen.y / 2,
                                                     screen.x / 2,
                                                     screen.y / 2), inScale);

                    // Animate the setting animation percentage
                    float targetAnim = mod->showSettings ? 1.f : 0.f;
                    mod->cAnim = MathUtils::animate(targetAnim, mod->cAnim, ImRenderUtils::getDeltaTime() * 12.5);
                    mod->cAnim = MathUtils::clamp(mod->cAnim, 0.f, 1.f);

                    // Settings
                    if (mod->cAnim > 0.001)
                    {
                        for (const auto& setting : mod->mSettings)
                        {
                            if (!setting->mIsVisible())
                            {
                                continue;
                            }

                            ImColor rgb = ColorUtils::getThemedColor(moduleY * 2);
                            // Base the alpha off the animation percentage
                            rgb.Value.w = animation;

                            switch (setting->mType)
                            {
                            case SettingType::Bool:
                                {
                                    BoolSetting* boolSetting = reinterpret_cast<BoolSetting*>(setting);
                                    moduleY = MathUtils::lerp(moduleY, moduleY + modHeight, mod->cAnim);

                                    ImVec4 rect = ImVec4(
                                            modRect.x, catPositions[i].y + catHeight + moduleY, modRect.z,
                                            catPositions[i].y + catHeight + moduleY + modHeight)
                                        .scaleToPoint(
                                            ImVec4(modRect.x, screen.y / 2,
                                                           modRect.z, screen.y / 2),
                                            inScale);

                                    if (rect.y > catRect.y + 0.5f)
                                    {
                                        std::string setName = setting->mName;
                                        ImRenderUtils::fillRectangle(rect, ImColor(30, 30, 30), animation);

                                        if (ImRenderUtils::isMouseOver(rect) && isEnabled)
                                        {
                                            tooltip = setting->mDescription;
                                            if (ImGui::IsMouseClicked(0))
                                            {
                                                //*(bool*)setting->getValue() = !*(bool*)setting->getValue();
                                                boolSetting->mValue = !boolSetting->mValue;
                                            }
                                        }

                                        setting->boolScale = MathUtils::animate(
                                            boolSetting->mValue ? 1 : 1, setting->boolScale,
                                            ImRenderUtils::getDeltaTime() * 10);

                                        float scaledWidth = rect.getWidth();
                                        float scaledHeight = rect.getHeight();
                                        ImVec2 center = ImVec2(
                                            rect.x + rect.getWidth() / 2.f, rect.y + rect.getHeight() / 2.f);
                                        ImVec4 scaledRect = ImVec4(
                                            center.x - scaledWidth / 2.f, center.y - scaledHeight / 2.f,
                                            center.x + scaledWidth / 2.f, center.y + scaledHeight / 2.f);

                                        ImVec4 smoothScaledRect = ImVec4(
                                            scaledRect.z - 19, scaledRect.y + 5, scaledRect.z - 5,
                                            scaledRect.w - 5);
                                        ImVec2 circleRect = ImVec2(
                                            smoothScaledRect.getCenter().x, smoothScaledRect.getCenter().y);

                                        ImRenderUtils::fillCircle(circleRect, 5,
                                                                 boolSetting->mValue
                                                                     ? rgb
                                                                     : ImColor(15, 15, 15), animation, 12.f);

                                        if (boolSetting->mValue) ImRenderUtils::fillShadowCircle(
                                            circleRect, 5, boolSetting->mValue ? rgb : ImColor(15, 15, 15),
                                            animation, 40, 0);

                                        float cSetRectCentreX = rect.x + ((rect.z - rect.x) -
                                            ImRenderUtils::getTextWidth(&setName, textSize)) / 2;
                                        float cSetRectCentreY = rect.y + ((rect.w - rect.y) - textHeight) / 2;
                                        ImRenderUtils::drawText(ImVec2(rect.x + 5.f, cSetRectCentreY), &setName,
                                                               ImColor(255, 255, 255), textSize, animation, true);
                                    }
                                    break;
                                }
                            case SettingType::Enum:
                                {
                                    EnumSetting* enumSetting = reinterpret_cast<EnumSetting*>(setting);
                                    std::string setName = setting->mName;
                                    std::vector<std::string> enumValues = enumSetting->mValues;
                                    int* iterator = &enumSetting->mValue;
                                    int numValues = static_cast<int>(enumValues.size());

                                    moduleY = MathUtils::lerp(moduleY, moduleY + modHeight, mod->cAnim);

                                    ImVec4 rect = ImVec4(
                                            modRect.x, catPositions[i].y + catHeight + moduleY, modRect.z,
                                            catPositions[i].y + catHeight + moduleY + modHeight)
                                        .scaleToPoint(
                                            ImVec4(modRect.x, screen.y / 2,
                                                           modRect.z, screen.y / 2),
                                            inScale);

                                    float targetAnim = setting->enumExtended && mod->showSettings ? 1.f : 0.f;
                                    setting->enumSlide = MathUtils::animate(
                                        targetAnim, setting->enumSlide, ImRenderUtils::getDeltaTime() * 10);
                                    setting->enumSlide = MathUtils::clamp(setting->enumSlide, 0.f, 1.f);

                                    if (setting->enumSlide > 0.001)
                                    {
                                        for (int j = 0; j < numValues; j++)
                                        {
                                            std::string enumValue = enumValues[j];

                                            moduleY = MathUtils::lerp(moduleY, moduleY + modHeight, setting->enumSlide);

                                            ImVec4 rect2 = ImVec4(
                                                    modRect.x, catPositions[i].y + catHeight + moduleY, modRect.z,
                                                    catPositions[i].y + catHeight + moduleY + modHeight)
                                                .scaleToPoint(
                                                    ImVec4(modRect.x, screen.y / 2,
                                                                   modRect.z, screen.y / 2),
                                                    inScale);

                                            if (rect2.y > catRect.y + 0.5f)
                                            {
                                                float cSetRectCentreY = rect2.y + ((rect2.w - rect2.y) - textHeight)
                                                    / 2;

                                                ImRenderUtils::fillRectangle(rect2, ImColor(20, 20, 20), animation);

                                                if (*iterator == j)
                                                    ImRenderUtils::fillRectangle(
                                                        ImVec4(rect2.x, rect2.y, rect2.x + 1.5f, rect2.w),
                                                        rgb, animation);

                                                if (ImRenderUtils::isMouseOver(rect2) && ImGui::IsMouseClicked(0) &&
                                                    isEnabled)
                                                {
                                                    *iterator = j;
                                                }

                                                ImRenderUtils::drawText(
                                                    ImVec2(rect2.x + 5.f, cSetRectCentreY), &enumValue,
                                                    ImColor(255, 255, 255), textSize, animation, true);
                                            }
                                        }
                                    }

                                    if (rect.y > catRect.y + 0.5f)
                                    {
                                        ImRenderUtils::fillRectangle(rect, ImColor(30, 30, 30), animation);

                                        if (ImRenderUtils::isMouseOver(rect) && isEnabled)
                                        {
                                            tooltip = setting->mDescription;
                                            if (ImGui::IsMouseClicked(0))
                                            {
                                                *iterator = (*iterator + 1) % enumValues.size();
                                            }
                                            else if (ImGui::IsMouseClicked(1) && mod->showSettings)
                                            {
                                                setting->enumExtended = !setting->enumExtended;
                                            }
                                        }

                                        float cSetRectCentreY = rect.y + ((rect.w - rect.y) - textHeight) / 2;

                                        std::string enumValue = enumValues[*iterator];
                                        std::string settingName = setName;
                                        std::string settingString = enumValue;
                                        auto ValueLen = ImRenderUtils::getTextWidth(&settingString, textSize);

                                        ImRenderUtils::drawText(ImVec2(rect.x + 5.f, cSetRectCentreY),
                                                               &settingName, ImColor(255, 255, 255), textSize,
                                                               animation, true);
                                        ImRenderUtils::drawText(
                                            ImVec2((rect.z - 5.f) - ValueLen, cSetRectCentreY),
                                            &settingString, ImColor(170, 170, 170), textSize, animation, true);
                                    }
                                    if (rect.y > catRect.y - modHeight)
                                    {
                                        ImRenderUtils::fillGradientOpaqueRectangle(
                                            ImVec4(rect.x, rect.w, rect.z,
                                                           rect.w + 10.f * setting->enumSlide * animation),
                                            ImColor(0, 0, 0), ImColor(0, 0, 0), 0.F * animation, 0.55F * animation);
                                    }
                                    break;
                                }
                            case SettingType::Number:
                                {
                                    NumberSetting* numSetting = reinterpret_cast<NumberSetting*>(setting);
                                    const float value = numSetting->mValue;
                                    const float min = numSetting->mMin;
                                    const float max = numSetting->mMax;

                                    char str[10];
                                    sprintf_s(str, 10, "%.1f", value);
                                    std::string rVal = str;

                                    std::string setName = setting->mName;
                                    std::string valueName = rVal;

                                    moduleY = MathUtils::lerp(moduleY, moduleY + modHeight, mod->cAnim);

                                    ImVec4 rect = ImVec4(
                                            modRect.x, (catPositions[i].y + catHeight + moduleY), modRect.z,
                                            catPositions[i].y + catHeight + moduleY + modHeight)
                                        .scaleToPoint(
                                            ImVec4(modRect.x, screen.y / 2,
                                                           modRect.z, screen.y / 2),
                                            inScale);

                                    if (rect.y > catRect.y + 0.5f)
                                    {
                                        ImRenderUtils::fillRectangle(rect, ImColor(30, 30, 30), animation);

                                        const float sliderPos = (value - min) / (max - min) * (rect.z - rect.x);

                                        setting->sliderEase = MathUtils::animate(
                                            sliderPos, setting->sliderEase, ImRenderUtils::getDeltaTime() * 10);
                                        setting->sliderEase = std::clamp(setting->sliderEase, 0.f, rect.getWidth());

                                        if (ImRenderUtils::isMouseOver(rect) && isEnabled)
                                        {
                                            tooltip = setting->mDescription;
                                            if (ImGui::IsMouseDown(0))
                                            {
                                                setting->isDragging = true;
                                            }
                                        }

                                        if (ImGui::IsMouseDown(0) && setting->isDragging && isEnabled)
                                        {
                                            const float newValue = std::fmax(
                                                std::fmin(
                                                    (ImRenderUtils::getMousePos().x - rect.x) / (rect.z - rect.x) * (
                                                        max - min) + min, max), min);
                                            numSetting->setValue(newValue);
                                        }
                                        else
                                        {
                                            setting->isDragging = false;
                                        }


                                        /* Original code (doesn't animate down when animating out)
                                        ImRenderUtils::fillRectangle(ImVec4(rect.x, (catPositions[i].y + catHeight + moduleY + modHeight) - 3, rect.x + setting->sliderEase, rect.w), rgb, animation);
                                        ImRenderUtils::fillShadowRectangle(ImVec4(rect.x, (catPositions[i].y + catHeight + moduleY + modHeight) - 3, rect.x + setting->sliderEase, rect.w), rgb, animation, 50.f, 0);*/

                                        float ySize = rect.w - rect.y;

                                        ImVec2 sliderBarMin = ImVec2(rect.x, rect.w - ySize / 8);
                                        ImVec2 sliderBarMax = ImVec2(rect.x + setting->sliderEase, rect.w);


                                        // The slider bar
                                        ImRenderUtils::fillRectangle(
                                            ImVec4(sliderBarMin.x, sliderBarMin.y, sliderBarMax.x,
                                                           sliderBarMax.y), rgb, animation);
                                        // Push a clip rect to prevent the shadow from going outside the slider bar
                                        ImGui::GetBackgroundDrawList()->PushClipRect(
                                            ImVec2(sliderBarMin.x, rect.y), ImVec2(sliderBarMax.x, sliderBarMax.y),
                                            true);

                                        ImRenderUtils::fillShadowRectangle(
                                            ImVec4(sliderBarMin.x, sliderBarMin.y, sliderBarMax.x,
                                                           sliderBarMax.y), rgb, animation, 50.f, 0);

                                        ImGui::GetBackgroundDrawList()->PopClipRect();

                                        auto ValueLen = ImRenderUtils::getTextWidth(&valueName, textSize);
                                        ImRenderUtils::drawText(
                                            ImVec2((rect.z - 5.f) - ValueLen, rect.y + 2.5f), &valueName,
                                            ImColor(170, 170, 170), textSize, animation, true);
                                        ImRenderUtils::drawText(ImVec2(rect.x + 5.f, rect.y + 2.5f),
                                                               &setName, ImColor(255, 255, 255), textSize,
                                                               animation, true);
                                    }
                                    break;
                                }
                            case SettingType::Color:
                                {
                                    ColorSetting* colorSetting = reinterpret_cast<ColorSetting*>(setting);
                                    ImColor color = colorSetting->getAsImColor();
                                    ImVec4 rgb = color.Value;
                                    std::string setName = setting->mName;

                                    moduleY = MathUtils::lerp(moduleY, moduleY + modHeight, mod->cAnim);

                                    ImVec4 rect = ImVec4(
                                            modRect.x, catPositions[i].y + catHeight + moduleY, modRect.z,
                                            catPositions[i].y + catHeight + moduleY + modHeight)
                                        .scaleToPoint(
                                            ImVec4(modRect.x, screen.y / 2,
                                                           modRect.z, screen.y / 2),
                                            inScale);

                                    if (rect.y > catRect.y + 0.5f)
                                    {
                                        ImRenderUtils::fillRectangle(rect, ImColor(30, 30, 30), animation);

                                        if (ImRenderUtils::isMouseOver(rect) && isEnabled)
                                        {
                                            tooltip = setting->mDescription;
                                            if (ImGui::IsMouseClicked(0))
                                            {
                                                displayColorPicker = !displayColorPicker;
                                                lastColorSetting = colorSetting;
                                            }
                                        }

                                        float cSetRectCentreY = rect.y + ((rect.w - rect.y) - textHeight) / 2;
                                        ImRenderUtils::drawText(ImVec2(rect.x + 5.f, cSetRectCentreY), &setName,
                                                               ImColor(255, 255, 255), textSize, animation, true);

                                        ImVec2 colorRect = ImVec2(rect.z - 20, rect.y + 5);
                                        ImRenderUtils::fillRectangle(ImVec4(rect.z - 20, rect.y + 5, rect.z - 5, rect.w - 5),
                                                                     colorSetting->getAsImColor(), animation);
                                    }
                                    break;
                                }
                            }
                        }
                    }


                    if (modRect.y > catRect.y + 0.5f)
                    {
                        // Draw the rect
                        if (mod->cScale <= 1)
                        {
                            ImRenderUtils::fillRectangle(modRect, ImColor(40, 40, 40), animation);
                        }

                        std::string modName = mod->mName;

                        // Calculate the centre of the rect
                        ImVec2 center = ImVec2(modRect.x + modRect.getWidth() / 2.f,
                                                               modRect.y + modRect.getHeight() / 2.f);

                        mod->cScale = MathUtils::animate(mod->mEnabled ? 1 : 0, mod->cScale,
                                                    ImRenderUtils::getDeltaTime() * 10);

                        // Calculate scaled size based on cScale
                        float scaledWidth = modRect.getWidth() * mod->cScale;
                        float scaledHeight = modRect.getHeight() * mod->cScale;

                        // Calculate new rectangle based on scaled size and center point
                        ImVec4 scaledRect = ImVec4(center.x - scaledWidth / 2.f,
                                                                   center.y - scaledHeight / 2.f,
                                                                   center.x + scaledWidth / 2.f,
                                                                   center.y + scaledHeight / 2.f);

                        // Interpolate between original rectangle and scaled rectangle
                        if (mod->cScale > 0)
                        {
                            ImRenderUtils::fillRectangle(scaledRect, rgb, animation * mod->cScale);
                            //ImRenderUtils::fillGradientOpaqueRectangle(scaledRect, ImColor(0, 0, 0), ImColor(0, 0, 0), 0.F * animation, 0.25F * animation);
                            //ImRenderUtils::fillGradientOpaqueRectangle(scaledRect, ColorUtils::RainbowDark(2.5, (moduleY * 2)), ColorUtils::RainbowDark(2.5, (moduleY * 2) + 400), 1, 1);
                            //ImRenderUtils::fillRectangle(Vector4(scaledRect.x, scaledRect.y, scaledRect.z, scaledRect.y + 1), ImColor(0, 0, 0), animation);
                        }

                        float cRectCentreX = modRect.x + ((modRect.z - modRect.x) - ImRenderUtils::getTextWidth(
                            &modName, textSize)) / 2;
                        float cRectCentreY = modRect.y + ((modRect.w - modRect.y) - textHeight) / 2;

                        // cRectCentreX. vRectCentreY
                        //.lerp(ImVec2(modRect.x + 5, cRectCentreY), mod->cAnim) // if we want lerp to left on extend
                        ImVec2 modPosLerped = ImVec2(cRectCentreX, cRectCentreY);

                        ImRenderUtils::drawText(modPosLerped, &modName,
                                               ImColor(mod->mEnabled
                                                           ? ImColor(255, 255, 255)
                                                           : ImColor(180, 180, 180)).Lerp(
                                                   ImColor(100, 100, 100), mod->cAnim), textSize, animation, true);

                        /*std::string bindName = (mod == lastMod && ClickGUIManager::isBinding) ? "Binding..." : std::to_string((char)mod->getKeybind()).c_str();
                        float bindNameLen = ImRenderUtils::getTextWidth(&bindName, textSize);
                        ImVec4 bindRect = ImVec4((modRect.z - 10) - bindNameLen, modRect.y + 2.5, modRect.z - 2.5, modRect.w - 2.5);
                        ImVec2 bindTextPos = ImVec2(bindRect.x + 3.5, cRectCentreY);

                        if (mod->getKeybind() != 7) {
                            ImRenderUtils::fillRectangle(bindRect, ImColor(29, 29, 29), 0.9, 4);
                            ImRenderUtils::drawText(bindTextPos, &bindName, ImColor(255, 255, 255), textSize, animation, true);
                        }
                        else if (mod->getKeybind() == 7 && mod == lastMod && ClickGUIManager::isBinding) {
                            ImRenderUtils::fillRectangle(bindRect, ImColor(29, 29, 29), 0.9, 4);
                            ImRenderUtils::drawText(bindTextPos, &bindName, ImColor(255, 255, 255), textSize, animation, true);
                        }*/

                        //GuiData* guidata = Global::getClientInstance()->getGuiData();
                        float renderx = screen.x / 2;
                        float rendery = (screen.y / 2) + 110;

                        if (ImRenderUtils::isMouseOver(modRect) && catPositions[i].isExtended && isEnabled)
                        {
                            if (ImRenderUtils::isMouseOver(catWindow) && catPositions[i].isExtended)
                            {
                                tooltip = mod->mDescription;
                            }

                            if (ImGui::IsMouseClicked(0))
                            {
                                mod->toggle();
                            }
                            else if (ImGui::IsMouseClicked(1))
                            {
                                // Only show if the module has settings
                                if (mod->mSettings.size() > 0) mod->showSettings = !mod->showSettings;
                            }
                            else if (ImGui::IsMouseClicked(2))
                            {
                                lastMod = mod;
                                isBinding = true;
                            }
                        }
                    }
                    if (modRect.y > catRect.y - modHeight)
                    {
                        // Render a slight glow effect
                        ImRenderUtils::fillGradientOpaqueRectangle(
                            ImVec4(modRect.x, modRect.w, modRect.z,
                                           modRect.w + 10.f * mod->cAnim * animation), ImColor(0, 0, 0),
                            ImColor(0, 0, 0), 0.F * animation, 0.55F * animation);
                    }
                    moduleY += modHeight;
                }
            }

            if (isBinding)
            {
                tooltip = "Currently binding " + lastMod->mName + "..." +" Press ESC to unbind.";
                for (const auto& key : Keyboard::mPressedKeys)
                {
                    if (key.second && lastMod)
                    {
                        //lastMod->setKeybind(key.first == Keys::ESC ? 7 : key.first);
                        lastMod->mKey = key.first;
                        isBinding = false;
                    }
                }
            }

            std::string catName = categories[i];

            if (ImRenderUtils::isMouseOver(catRect) && ImGui::IsMouseClicked(1))
                catPositions[i].isExtended = !catPositions[i].isExtended;

            ImRenderUtils::fillRectangle(catRect, ImColor(20, 20, 20), animation);

            // Calculate the centre of the rect
            float cRectCentreX = catRect.x + ((catRect.z - catRect.x) - ImRenderUtils::getTextWidth(
                &catName, textSize * 1.15)) / 2;
            float cRectCentreY = catRect.y + ((catRect.w - catRect.y) - textHeight) / 2;

            // Draw the string
            ImRenderUtils::drawText(ImVec2(cRectCentreX, cRectCentreY), &catName, ImColor(255, 255, 255),
                                   textSize * 1.15, animation, true);

            catPositions[i].x = std::clamp(catPositions[i].x, 0.f, screen.x - catWidth);
            catPositions[i].y = std::clamp(catPositions[i].y, 0.f, screen.y - catHeight);

#pragma region DraggingLogic
            static bool dragging = false;
            static ImVec2 dragOffset;
            if (catPositions[i].isDragging)
            {
                if (ImGui::IsMouseDown(0))
                {
                    if (!dragging)
                    {
                        dragOffset = ImVec2(ImRenderUtils::getMousePos().x - catRect.x,
                                                    ImRenderUtils::getMousePos().y - catRect.y);
                        dragging = true;
                    }
                    ImVec2 newPosition = ImVec2(ImRenderUtils::getMousePos().x - dragOffset.x,
                                                                ImRenderUtils::getMousePos().y - dragOffset.y);
                    newPosition.x = std::clamp(newPosition.x, 0.f,
                                               screen.x - catWidth);
                    newPosition.y = std::clamp(newPosition.y, 0.f,
                                               screen.y - catHeight);
                    catPositions[i].x = newPosition.x;
                    catPositions[i].y = newPosition.y;
                }
                else
                {
                    catPositions[i].isDragging = false;
                    dragging = false;
                }
            }
            else if (ImRenderUtils::isMouseOver(catRect) && ImGui::IsMouseClicked(0) && isEnabled)
            {
                catPositions[i].isDragging = true;
                dragOffset = ImVec2(ImRenderUtils::getMousePos().x - catRect.x,
                                            ImRenderUtils::getMousePos().y - catRect.y);
            }
#pragma endregion
        }

        if (!tooltip.empty())
        {
            float textWidth = ImRenderUtils::getTextWidth(&tooltip, textSize * 0.8f);
            float textHeight = ImRenderUtils::getTextHeight(textSize * 0.8f);
            float padding = 2.5f;
            float offset = 8.f;

            ImVec4 tooltipRect = ImVec4(
                ImRenderUtils::getMousePos().x + offset - padding,
                ImRenderUtils::getMousePos().y + textHeight / 2 - textHeight - padding,
                ImRenderUtils::getMousePos().x + offset + textWidth + padding * 2,
                ImRenderUtils::getMousePos().y + textHeight / 2 + padding
            ).scaleToPoint(ImVec4(
                               screen.x / 2,
                               screen.y / 2,
                               screen.x / 2,
                               screen.y / 2
                           ), inScale);

            ImRenderUtils::fillRectangle(tooltipRect, ImColor(20, 20, 20), animation, 0.f);
            ImRenderUtils::drawText(ImVec2(tooltipRect.x + padding, tooltipRect.y + padding), &tooltip,
                                   ImColor(255, 255, 255), textSize * 0.8f, animation, true);
        }

        if (isEnabled)
        {
            scrollDirection = 0;
        }
    }
    ImGui::PopFont();
}
