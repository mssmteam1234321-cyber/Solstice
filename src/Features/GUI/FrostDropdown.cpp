//
// Created by Tozic on 8/7/2024.
//

#include "FrostDropdown.hpp"
#include <Features/Modules/ModuleCategory.hpp>
#include <Features/Modules/Visual/ClickGui.hpp>
#include <Utils/FontHelper.hpp>
#include <Utils/MiscUtils/ImRenderUtils.hpp>
#include <Utils/MiscUtils/MathUtils.hpp>
#include <Features/Modules/Setting.hpp>
#include <Features/Modules/Visual/Interface.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Rendering/GuiData.hpp>
#include <Utils/Keyboard.hpp>
#include <Utils/StringUtils.hpp>
#include <Utils/MiscUtils/ColorUtils.hpp>

ImVec4 FrostGui::scaleToPoint(const ImVec4& _this, const ImVec4& point, float amount)
{
    return {point.x + (_this.x - point.x) * amount, point.y + (_this.y - point.y) * amount,
        point.z + (_this.z - point.z) * amount, point.w + (_this.w - point.w) * amount };
}

bool FrostGui::isMouseOver(const ImVec4& rect)
{
    ImVec2 mousePos = ImGui::GetIO().MousePos;
    return mousePos.x >= rect.x && mousePos.y >= rect.y && mousePos.x < rect.z && mousePos.y < rect.w;
}

ImVec4 FrostGui::getCenter(ImVec4& vec)
{
    float centerX = (vec.x + vec.z) / 2.0f;
    float centerY = (vec.y + vec.w) / 2.0f;
    return { centerX, centerY, centerX, centerY };
}

void FrostGui::render(float animation, float inScale, int& scrollDirection, char* h, float blur, float midclickRounding)
{
    static auto interfaceMod = gFeatureManager->mModuleManager->getModule<Interface>();
    bool lowercase = interfaceMod->mNamingStyle.mValue == NamingStyle::Lowercase || interfaceMod->mNamingStyle.mValue == NamingStyle::LowercaseSpaced;

    FontHelper::pushPrefFont(true);
    ImVec2 screen = ImRenderUtils::getScreenSize();
    float deltaTime = ImGui::GetIO().DeltaTime;
    auto drawList = ImGui::GetBackgroundDrawList();

    if (resetPosition && NOW - lastReset > 100)
    {
        catPositions.clear();
        ImVec2 screen = ImRenderUtils::getScreenSize();
        auto categories = ModuleCategoryNames;
        if (catPositions.empty())
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
        resetPosition = false;
    }

    /*ImRenderUtils::fillRectangle(
            ImVec4(0, 0, screen.x, screen.y),
            ImColor(0, 0, 0), animation * 0.38f);*/
    drawList->AddRectFilled(ImVec2(0, 0), ImVec2(screen.x, screen.y), IM_COL32(0, 0, 0, 255 * animation * 0.38f));
    ImRenderUtils::addBlur(ImVec4(0.f, 0.f, screen.x, screen.y),
                           animation * blur, 0);

    // Draw a glow rect on the bottom 1/3 of the screen
    ImColor shadowRectColor = ColorUtils::getThemedColor(0);
    shadowRectColor.Value.w = 0.5f * animation;

    // Draw a gradient rect on the bottom 1/3 of the screen with the shadow color, and making the alpha taper off as it goes up
    float firstheight = (screen.y - screen.y / 3);
    // As the animation reaches 0, the height should go below the screen height
    firstheight = MathUtils::lerp(screen.y, firstheight, inScale);
    ImRenderUtils::fillGradientOpaqueRectangle(
        ImVec4(0, firstheight, screen.x, screen.y),
        shadowRectColor, shadowRectColor, 0.4f * inScale, 0.0f);



    static std::vector<std::string> categories = ModuleCategoryNames;
    static std::vector<std::shared_ptr<Module>>& modules = gFeatureManager->mModuleManager->getModules();

    static auto* clickGui = gFeatureManager->mModuleManager->getModule<ClickGui>();
    bool isEnabled = clickGui->mEnabled;
    std::string tooltip = "";

    float textSize = inScale;
    //float textHeight = ImRenderUtils::getTextHeight(textSize);
    float textHeight = ImGui::GetFont()->CalcTextSizeA(textSize * 18, FLT_MAX, -1, "").y;

    if (displayColorPicker && isEnabled)
    {
        FontHelper::pushPrefFont(false);
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
                std::string modLower = mod->getName();

                std::transform(modLower.begin(), modLower.end(), modLower.begin(), [](unsigned char c)
                {
                    return std::tolower(c);
                });

                for (const auto& setting : mod->mSettings)
                {
                    if (!setting->mIsVisible()) // need this cuz of fucking overriding shit 🙃
                    {
                        //Reset the animation if the setting is not visible
                        setting->sliderEase = 0;
                        setting->enumSlide = 0;
                        continue;
                    }

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

                            if (setting->colourSlide > 0.01)
                            {
                                settingsHeight = MathUtils::lerp(settingsHeight, settingsHeight + colorPickerHeight, setting->colourSlide); // Sigma king tozic
                            }
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

            ImRenderUtils::fillRectangle(catWindow, settingsBackgroundColor, animation, 15);

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

            int ModulesCount = 0; // Erm, what the sigma???

            for (const auto& mod : modsInCategory)
            {
                ModulesCount++;

                float ModuleRounding = 0.f;

                if (ModulesCount == modsInCategory.size() && !mod->showSettings)
                    ModuleRounding = 15.f;

                std::string modLower = mod->getName();

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
                        static bool wasDragging = false;
                        Setting* lastDraggedSetting = nullptr;
                        for (const auto& setting : mod->mSettings)
                        {
                            if (!setting->mIsVisible())
                            {
                                //Reset the animation if the setting is not visible
                                setting->sliderEase = 0;
                                setting->enumSlide = 0;
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
                                        std::string setName = lowercase ? StringUtils::toLower(setting->mName) : setting->mName;
                                        //ImRenderUtils::fillRectangle(rect, ImColor(30, 30, 30), animation); // I can't round this so doesn't matter ima do a big fat rectangle for the background

                                        if (ImRenderUtils::isMouseOver(rect) && isEnabled)
                                        {
                                            tooltip = setting->mDescription;
                                            if (ImGui::IsMouseClicked(0))
                                            {
                                                boolSetting->mValue = !boolSetting->mValue;
                                            }
                                        }

                                        setting->boolScale = MathUtils::animate(
                                            boolSetting->mValue ? 1 : 1, setting->boolScale,
                                            ImRenderUtils::getDeltaTime() * 10);

                                        // y not ease them?
                                        float scaledWidth = rect.getWidth();
                                        float scaledHeight = rect.getHeight();

                                        ImVec2 center = ImVec2(rect.x + rect.getWidth() / 2.f, rect.y + rect.getHeight() / 2.f);
                                        ImVec4 scaledRect = ImVec4(center.x - scaledWidth / 2.f, center.y - scaledHeight / 2.f, center.x + scaledWidth / 2.f, center.y + scaledHeight / 2.f);

                                        float cSetRectCentreX = rect.x + ((rect.z - rect.x) - ImRenderUtils::getTextWidth(&setName, textSize)) / 2;
                                        float cSetRectCentreY = rect.y + ((rect.w - rect.y) - textHeight) / 2;

                                        ImVec4 smoothScaledRect = ImVec4(scaledRect.z - 19, scaledRect.y + 5, scaledRect.z - 5, scaledRect.w - 5);//
                                        ImVec2 circleRect = ImVec2(smoothScaledRect.getCenter().x, smoothScaledRect.getCenter().y);

                                        ImRenderUtils::fillShadowCircle(circleRect, 5, boolSetting->mValue ? rgb : ImColor(15, 15, 15), animation, 40, 0);

                                        // Very sigma of me 😈
                                        ImVec4 booleanRect = ImVec4(rect.z - 23.5f, cSetRectCentreY - 2.5f, rect.z - 5, cSetRectCentreY + 17.5f);

                                        if (boolSetting->mValue)
                                        {
                                            ImRenderUtils::drawCheckMark(ImVec2(booleanRect.getCenter().x - 4, booleanRect.getCenter().y - 1), 1.3, rgb, mod->cAnim);
                                            ImRenderUtils::drawCheckMark(ImVec2(booleanRect.getCenter().x - 4,  booleanRect.getCenter().y - 1), 1.3, rgb, mod->cAnim);
                                        }

                                        ImRenderUtils::drawText(ImVec2(rect.x + 5.f, cSetRectCentreY), setName,
                                                               ImColor(255, 255, 255), textSize, animation, true);
                                    }
                                    break;
                                }
                            case SettingType::Enum:
                                {
                                    EnumSetting* enumSetting = reinterpret_cast<EnumSetting*>(setting);
                                    std::string setName = lowercase ? StringUtils::toLower(setting->mName) : setting->mName;
                                    std::vector<std::string> enumValues = enumSetting->mValues;
                                    if (lowercase)
                                    {
                                        for (std::string& value : enumValues)
                                        {
                                            value = StringUtils::toLower(value);
                                        }
                                    }
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
                                    setting->enumSlide = 0;
                                    setting->enumSlide = 0;

                                    if (rect.y > catRect.y + 0.5f)
                                    {
                                        if (ImRenderUtils::isMouseOver(rect) && isEnabled)
                                        {
                                            tooltip = setting->mDescription;
                                            if (ImGui::IsMouseClicked(0))
                                            {
                                                *iterator = (*iterator + 1) % enumValues.size();
                                            }
                                            else if (ImGui::IsMouseClicked(1) && mod->showSettings)
                                            {
                                                *iterator = (*iterator - 1) % enumValues.size();
                                            }
                                        }

                                        float cSetRectCentreY = rect.y + ((rect.w - rect.y) - textHeight) / 2;

                                        std::string enumValue = enumValues[*iterator];
                                        std::string settingName = setName;
                                        std::string settingString = enumValue;
                                        auto ValueLen = ImRenderUtils::getTextWidth(&settingString, textSize);

                                        ImRenderUtils::drawText(ImVec2(rect.x + 5.f, cSetRectCentreY),
                                                               settingName, ImColor(255, 255, 255), textSize,
                                                               animation, true);
                                        ImRenderUtils::drawText(
                                            ImVec2((rect.z - 5.f) - ValueLen, cSetRectCentreY),
                                            settingString, ImColor(170, 170, 170), textSize, animation, true);
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
                                    sprintf_s(str, 10, "%.2f", value);
                                    std::string rVal = str;

                                    std::string setName = lowercase ? StringUtils::toLower(setting->mName) : setting->mName;
                                    std::string valueName = rVal;

                                    moduleY = MathUtils::lerp(moduleY, moduleY + modHeight, mod->cAnim);

                                    ImVec4 backGroundRect = ImVec4(
                                            modRect.x, (catPositions[i].y + catHeight + moduleY), modRect.z,
                                            catPositions[i].y + catHeight + moduleY + modHeight)
                                        .scaleToPoint(
                                            ImVec4(modRect.x, screen.y / 2,
                                                           modRect.z, screen.y / 2),
                                            inScale);

                                    ImVec4 rect = ImVec4(
                                            modRect.x + 7, (catPositions[i].y + catHeight + moduleY), modRect.z - 7,
                                            catPositions[i].y + catHeight + moduleY + modHeight)
                                        .scaleToPoint(
                                            ImVec4(modRect.x, screen.y / 2,
                                                           modRect.z, screen.y / 2),
                                            inScale);

                                    static float clickAnimation = 1.f;

                                    // If left click is down, lerp the alpha to 0.60f;
                                    if (ImGui::IsMouseDown(0) && ImRenderUtils::isMouseOver(rect))
                                    {
                                        clickAnimation = MathUtils::animate(0.60f, clickAnimation, ImRenderUtils::getDeltaTime() * 10);
                                    }
                                    else
                                    {
                                        clickAnimation = MathUtils::animate(1.f, clickAnimation, ImRenderUtils::getDeltaTime() * 10);
                                    }

                                    if (backGroundRect.y > catRect.y + 0.5f)
                                    {
                                        //ImRenderUtils::fillRectangle(backGroundRect, ImColor(30, 30, 30), animation);

                                        const float sliderPos = (value - min) / (max - min) * (rect.z - rect.x);

                                        setting->sliderEase = MathUtils::animate(
                                            sliderPos, setting->sliderEase, ImRenderUtils::getDeltaTime() * 10);
                                        setting->sliderEase = std::clamp(setting->sliderEase, 0.f, rect.getWidth());

#pragma region Slider dragging
                                       if (ImRenderUtils::isMouseOver(rect) && isEnabled)
                                        {
                                            tooltip = setting->mDescription;
                                            if (ImGui::IsMouseDown(0) || ImGui::IsMouseDown(2))
                                            {
                                                setting->isDragging = true;
                                                lastDraggedSetting = setting;
                                            }
                                        }

                                        if (ImGui::IsMouseDown(0) && setting->isDragging && isEnabled)
                                        {
                                            if (lastDraggedSetting != setting)
                                            {
                                                setting->isDragging = false;
                                            } else
                                            {
                                                const float newValue = std::fmax(
                                                    std::fmin(
                                                        (ImRenderUtils::getMousePos().x - rect.x) / (rect.z - rect.x) * (
                                                            max - min) + min, max), min);
                                                numSetting->setValue(newValue);
                                            }
                                        }
                                        else if (ImGui::IsMouseDown(2) && setting->isDragging && isEnabled)
                                        {
                                            if (lastDraggedSetting != setting)
                                            {
                                                setting->isDragging = false;
                                            } else
                                            {
                                                float newValue = std::fmax(
                                                    std::fmin(
                                                        (ImRenderUtils::getMousePos().x - rect.x) / (rect.z - rect.x) * (
                                                            max - min) + min, max), min);
                                                // Round the value to the nearest value specified by midclickRounding
                                                newValue = std::round(newValue / midclickRounding) * midclickRounding;
                                                numSetting->mValue = newValue;
                                            }
                                        }
                                        else
                                        {
                                            setting->isDragging = false;
                                        }
#pragma endregion

                                        // "doesn't animate down when animating out" cuz its made by a 11 yr old smart nerd :yum:
                                        /* Original code (doesn't animate down when animating out)
                                        ImRenderUtils::fillRectangle(ImVec4(rect.x, (catPositions[i].y + catHeight + moduleY + modHeight) - 3, rect.x + setting->sliderEase, rect.w), rgb, animation);
                                        ImRenderUtils::fillShadowRectangle(ImVec4(rect.x, (catPositions[i].y + catHeight + moduleY + modHeight) - 3, rect.x + setting->sliderEase, rect.w), rgb, animation, 50.f, 0);*/

                                        float ySize = rect.w - rect.y;

                                        ImVec2 sliderBarMin = ImVec2(rect.x, rect.w - ySize / 8);
                                        ImVec2 sliderBarMax = ImVec2(rect.x + (setting->sliderEase * inScale), rect.w);
                                        sliderBarMin.y = sliderBarMax.y - 4 * inScale;

                                        ImVec4 sliderRect = ImVec4(sliderBarMin.x, sliderBarMin.y - 4.5f, sliderBarMax.x, sliderBarMax.y - 6.5f);

                                        // The slider bar
                                        ImRenderUtils::fillRectangle(sliderRect, rgb, animation, 15);

                                        // Circle (I am not sure)
                                        ImVec2 circlePos = ImVec2(sliderRect.z - 2.25f, sliderRect.getCenter().y);

                                        if (value <= min + 0.83f)
                                        {
                                            circlePos.x = sliderRect.z + 2.25f;
                                        }

                                        ImRenderUtils::fillCircle(circlePos, 5.5f * clickAnimation, rgb, animation, 12);

                                        // Push a clip rect to prevent the shadow from going outside the slider bar
                                        ImGui::GetBackgroundDrawList()->PushClipRect(ImVec2(sliderRect.x, sliderRect.y), ImVec2(sliderRect.z, sliderRect.w), true);

                                        ImRenderUtils::fillShadowRectangle(sliderRect, rgb, animation * 0.75f, 15.f, 0);

                                        ImGui::GetBackgroundDrawList()->PopClipRect();

                                        auto ValueLen = ImRenderUtils::getTextWidth(&valueName, textSize);
                                        ImRenderUtils::drawText(
                                            ImVec2((backGroundRect.z - 5.f) - ValueLen, backGroundRect.y + 2.5f), valueName,
                                            ImColor(170, 170, 170), textSize, animation, true);
                                        ImRenderUtils::drawText(ImVec2(backGroundRect.x + 5.f, backGroundRect.y + 2.5f),
                                                               setName, ImColor(255, 255, 255), textSize,
                                                               animation, true);
                                    }
                                    break;
                                }
                            case SettingType::Color:
                                {
                                    ColorSetting* colorSetting = reinterpret_cast<ColorSetting*>(setting);
                                    ImColor color = colorSetting->getAsImColor();
                                    static ImVec4 rgb = color.Value;
                                    std::string setName = lowercase ? StringUtils::toLower(setting->mName) : setting->mName;

                                    moduleY = MathUtils::lerp(moduleY, moduleY + modHeight, mod->cAnim);

                                    ImVec4 rect = ImVec4(
                                            modRect.x, catPositions[i].y + catHeight + moduleY, modRect.z,
                                            catPositions[i].y + catHeight + moduleY + modHeight)
                                        .scaleToPoint(
                                            ImVec4(modRect.x, screen.y / 2,
                                                           modRect.z, screen.y / 2),
                                            inScale);

                                    // Change of plans THIS LOOKS HORRIBLE when Sliding so I am removing the slide animations

                                    float targetAnim = setting->colourExtended && mod->showSettings ? 1.f : 0.f;
                                    setting->colourSlide = targetAnim;
                                    /*setting->colourSlide = MathUtils::animate(
                                        targetAnim, setting->colourSlide, ImRenderUtils::getDeltaTime() * 10);
                                    setting->colourSlide = MathUtils::clamp(setting->colourSlide, 0.f, 1.f);*/

                                    if (setting->colourSlide > 0.001)
                                    {
                                        moduleY = MathUtils::lerp(moduleY, moduleY + colorPickerHeight, setting->colourSlide);

                                        ImVec4 colourRect = ImVec4(
                                                    rect.x + 4, rect.w + 4, 185.f,
                                                    61.f);


                                        if (colourRect.y > catRect.y + 0.5f)
                                        {
                                            ImRenderUtils::drawColorPicker(rgb, colourRect);
                                            colorSetting->setColor(rgb.x, rgb.y, rgb.z, rgb.w);
                                        }
                                    }

                                    if (rect.y > catRect.y + 0.5f)
                                    {
                                        if (ImRenderUtils::isMouseOver(rect) && isEnabled)
                                        {
                                            tooltip = setting->mDescription;
                                            if (ImGui::IsMouseClicked(1))
                                            {
                                                setting->colourExtended = !setting->colourExtended;
                                            }
                                        }

                                        float cSetRectCentreY = rect.y + ((rect.w - rect.y) - textHeight) / 2;
                                        ImRenderUtils::drawText(ImVec2(rect.x + 5.f, cSetRectCentreY), setName,
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

                        modRect.y -= 1;

                        if (mod->cScale <= 1)
                        {
                            ImRenderUtils::fillRectangle(modRect, moduleBackgroundColor, animation, ModuleRounding, ImGui::GetBackgroundDrawList(), ImDrawFlags_RoundCornersBottom);
                        }

                        std::string modName = mod->getName();

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

                            //ImRenderUtils::fillRectangle(scaledRect, rgb, animation * mod->cScale + 0.01f);
                            ImColor rgb1 = rgb;
                            float modIndexY = moduleY + (scaledRect.y - scaledRect.w);

                            ImColor rgb2 = ColorUtils::getThemedColor(scaledRect.y + ((scaledRect.z - scaledRect.x)));
                            //ImRenderUtils::fillGradientOpaqueRectangle(scaledRect, rgb1, rgb2, animation, animation); // Erm, what the sigma...

                            scaledRect.y -= 1;
                            ImRenderUtils::fillRectangle(scaledRect, rgb, animation, ModuleRounding, ImGui::GetBackgroundDrawList(), ImDrawFlags_RoundCornersBottom); // very sigma

                            // UnCommit this line under if u want gradient
                            //ImRenderUtils::fillRoundedGradientRectangle(scaledRect, rgb1, rgb2, ModuleRounding, animation, animation, ImDrawFlags_RoundCornersBottom);
                        }

                        float cRectCentreX = modRect.x + ((modRect.z - modRect.x) - ImRenderUtils::getTextWidth(
                            &modName, textSize)) / 2;
                        float cRectCentreY = modRect.y + ((modRect.w - modRect.y) - textHeight) / 2;

                        // cRectCentreX. vRectCentreY
                        //.lerp(ImVec2(modRect.x + 5, cRectCentreY), mod->cAnim) // if we want lerp to left on extend
                        ImVec2 modPosLerped = ImVec2(cRectCentreX, cRectCentreY);

                        ImRenderUtils::drawText(modPosLerped, modName,
                                               ImColor(mod->mEnabled
                                                           ? ImColor(255, 255, 255)
                                                           : ImColor(180, 180, 180)).Lerp(
                                                           mod->mEnabled
                                                           ? ImColor(255, 255, 255)
                                                           : ImColor(180, 180, 180), mod->cAnim), textSize, animation, true);

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
                                ClientInstance::get()->playUi("random.pop", 0.75f, 1.0f);
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
                tooltip = "Currently binding " + lastMod->getName() + "..." +" Press ESC to unbind.";
                for (const auto& key : Keyboard::mPressedKeys)
                {
                    if (key.second && lastMod)
                    {
                        //lastMod->setKeybind(key.first == Keys::ESC ? 7 : key.first);
                        lastMod->mKey = key.first == VK_ESCAPE ? 0 : key.first;
                        isBinding = false;
                        if (key.first == VK_ESCAPE)
                        {
                            ClientInstance::get()->playUi("random.break", 0.75f, 1.0f);
                        } else
                        {
                            ClientInstance::get()->playUi("random.orb", 0.75f, 1.0f);
                        }
                    }
                }
            }

            std::string catName = lowercase ? StringUtils::toLower(categories[i]) : categories[i];

            if (ImRenderUtils::isMouseOver(catRect) && ImGui::IsMouseClicked(1))
                catPositions[i].isExtended = !catPositions[i].isExtended;

            ImRenderUtils::fillRectangle(catRect, categoryHeaderColor, animation, 15, ImGui::GetBackgroundDrawList(), ImDrawFlags_RoundCornersTop);

            // Calculate the centre of the rect
            FontHelper::pushPrefFont(true, true);
            float textHeight = ImGui::GetFont()->CalcTextSizeA(textSize * 18, FLT_MAX, -1, catName.c_str()).y;
            float cRectCentreX = catRect.x + ((catRect.z - catRect.x) - ImRenderUtils::getTextWidth(
                &catName, textSize * 1.15)) / 2;
            float cRectCentreY = catRect.y + ((catRect.w - catRect.y) - textHeight) / 2;

            ImRenderUtils::drawText(ImVec2(cRectCentreX, cRectCentreY), catName, ImColor(255, 255, 255),
                                   textSize * 1.14, animation, false);
            ImGui::PopFont();

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
            ImVec2 toolTipHeight = ImGui::GetFont()->CalcTextSizeA(textSize * 14.4f, FLT_MAX, 0, tooltip.c_str());
            float textWidth = ImRenderUtils::getTextWidth(&tooltip, textSize * 0.8f);
            float textHeight = toolTipHeight.y;
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

            static float alpha = 1.f;

            // If mid or left click is down, lerp the alpha to 0.25f;
            if (ImGui::IsMouseDown(0) || ImGui::IsMouseDown(2))
            {
                alpha = MathUtils::animate(0.0f, alpha, ImRenderUtils::getDeltaTime() * 10);
            }
            else
            {
                alpha = MathUtils::animate(1.f, alpha, ImRenderUtils::getDeltaTime() * 10);
            }

            ImRenderUtils::fillRectangle(tooltipRect, ImColor(20, 20, 20), animation * alpha, 0.f);
            ImRenderUtils::drawText(ImVec2(tooltipRect.x + padding, tooltipRect.y + padding), tooltip,
                                   ImColor(255, 255, 255), textSize * 0.8f, animation * alpha, true);
        }

        if (isEnabled)
        {
            scrollDirection = 0;
        }
    }
    ImGui::PopFont();
}

void FrostGui::onWindowResizeEvent(WindowResizeEvent& event)
{
    resetPosition = true;
    lastReset = NOW;
}
