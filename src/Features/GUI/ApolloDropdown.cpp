/*
//
// Created by vastrakai on 7/20/2024.
//

#include "ApolloDropdown.hpp"

#include <Features/Modules/Visual/ClickGui.hpp>


void ApolloGUI::render(float animation, float inScale, int& scrollDirection) noexcept
{

    const glm::vec2<float> guiPos = glm::vec2<float>(ImRenderUtils::getScreenSize().x / 2, ImRenderUtils::getScreenSize().y / 2);
    ImRenderUtils::fillRectangle(ImVec4(0, 0, ImRenderUtils::getScreenSize().x, ImRenderUtils::getScreenSize().y), ImColor(0, 0, 0), animation * 0.2f);
    ImRenderUtils::addBlur(ImVec4(0, 0, ImRenderUtils::getScreenSize().x, ImRenderUtils::getScreenSize().y), animation * 9, 0);
    static auto* clickGui = gFeatureManager->mModuleManager->getModule<ClickGui>();
    bool isEnabled = clickGui->mEnabled;

    static std::vector<std::string> categories = ModuleCategoryNames;
    static std::vector<std::shared_ptr<Module>>& modules = gFeatureManager->mModuleManager->getModules();

    if (catPositions.empty() && isEnabled)
    {
        float centerX = ImRenderUtils::getScreenSize().x / 2;
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

    if (!catPositions.empty()) {
        scrollSize = -scrollDirection * catHeight;
        for (size_t i = 0; i < categories.size(); i++) {
            catPositions[i].y += scrollSize;
            ImVec4 catTabRect = ImVec4(catPositions[i].x, catPositions[i].y, catPositions[i].x + catWidth, catPositions[i].y + catHeight);
            ImVec2 catNameRect = ImVec2(catPositions[i].x + 7.f, catPositions[i].y + 7.f);

            ImRenderUtils::fillShadowRectangle(catPositions[i].shadowRect, mainBlack, 1.f, 70.f, ImDrawFlags_RoundCornersAll, 10.f);
            ImRenderUtils::fillShadowRectangle(catPositions[i].shadowRect, mainBlack, 1.f, 70.f, ImDrawFlags_RoundCornersAll, 10.f);

            const float modWidth = catWidth;
            const float modHeight = catHeight;
            float moduleY = 0;
            const auto& modsInCategory = gFeatureManager->mModuleManager->getModulesInCategory(i);
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
            int mcount = 0;
            for (const auto& mod : modsInCategory) {
                mcount++;
                float myHeight = modHeight;
                ImVec4 modRect = ImVec4(catPositions[i].x, catPositions[i].y + moduleY + catHeight, catPositions[i].x + modWidth, catPositions[i].y + moduleY + catHeight + modHeight);
                ImVec2 modNameRect = ImVec2(modRect.x + 7.5f, modRect.y + 7.5f);
                ImColor modColor = mainBlack;
                ImColor modSecColor = mainBlack;
                if (mod->mEnabled) {
                    modColor = ColorUtils::getClientColor(1.9, 1, 1, 0);
                    modSecColor = ColorUtils::getClientColor(1.9, 1, 1, 180);
                }
                float round = 0.f;
                if (mcount == modsInCategory.size() && !mod->showSettings) round = 12.f;
                ImRenderUtils::fillRoundedGradientRectangle(modRect, modColor, modSecColor, round, 1.f, 1.f, ImDrawFlags_RoundCornersBottom);
                std::string modName = mod->getName();
                ImRenderUtils::drawText(modNameRect, &modName, textColor, 1.2f);

                if (ImRenderUtils::isMouseOver(modRect) && isEnabled) {
                    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                        mod->toggle();
                    } else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                        mod->showSettings = !mod->showSettings;
                    }
                }

                ImVec4 backgroundRect = ImVec4(modRect.x, modRect.w, modRect.z, modRect.w + mod->settingsScale);
                round = 0.f;
                if (mcount == modsInCategory.size() && mod->showSettings) round = 12.f;
                ImRenderUtils::fillRectangle(backgroundRect, mainBlack, 1.f, round, ImDrawFlags_RoundC  ornersBottom);
                float settingsHeight = 0.f;
                if (mod->showSettings) {
                    float padding = 5.f;
                    ImClippingUtil::beginClipping(Vector4<float>(modRect.x, modRect.w, modRect.z, modRect.w + mod->settingsScale));
                    for (const auto& setting : mod->mSettings) {
                        if (!setting->render) continue;
                        float settingHeight = 0.f;
                        setting->renderEnumSlide = MathUtils::animate(setting->enumSlide, setting->renderEnumSlide, ImRenderUtils::getDeltaTime() * 10);
                        if (setting->getType() == SettingType::BOOL) settingHeight = modHeight + padding;
                        else if (setting->getType() == SettingType::ENUM) settingHeight = modHeight * 1.5 + padding;
                        else if (setting->getType() == SettingType::SLIDER) settingHeight = modHeight + padding;
                        Vector4<float> settingRect = Vector4<float>(modRect.x, modRect.w + settingsHeight, modRect.z, modRect.w + settingsHeight + settingHeight);
                        Vector2<float> settingCenter = Vector2<float>((settingRect.x + settingRect.z) / 2, (settingRect.y + settingRect.w) / 2);
                        modColor = ColorUtils::getClientColor(1.9, 1, 1, 0);
                        modSecColor = ColorUtils::getClientColor(1.9, 1, 1, 180);
                        settingHeight += setting->renderEnumSlide;

                        switch (setting->getType()) {
                        case SettingType::BOOL: {
                            Vector4<float> buttonRect = Vector4<float>(settingCenter.x - modWidth / 2 + 10.f, settingCenter.y - 15, settingCenter.x + modWidth / 2 - 10.f, settingCenter.y + 15);
                            if (*(bool*)setting->getValue()) {
                                ImRenderUtils::drawRoundedGradientRectangleOutline(buttonRect, modColor, modSecColor, 6.f, 1.f, 1.f, 2.f);
                            } else {
                                ImRenderUtils::drawRoundRect(buttonRect, ImDrawFlags_None, 6.f, grayBlack, 1.f, 2.f);
                            }

                            std::string settingName = setting->getName();
                            float textSizeX = ImRenderUtils::getTextWidth(&settingName, 1.f);
                            float textSizeY = ImRenderUtils::getTextHeightStr(&settingName, 1.f);
                            Vector2<float> settingTextRect = Vector2<float>(settingCenter.x - textSizeX / 2, settingCenter.y - textSizeY / 2);
                            ImRenderUtils::drawText(settingTextRect, &settingName, textColor, 1.f);

                            if (ImRenderUtils::isMouseOver(buttonRect) && isEnabled) {
                                if (Utils::leftClick) {
                                    *(bool*)setting->value = !*(bool*)setting->value;
                                    Utils::leftClick = false;
                                }
                            }
                            break;
                        }
                        case SettingType::ENUM: {
                            Vector4<float> buttonRect = Vector4<float>(settingCenter.x - modWidth / 2 + 10.f, settingCenter.y - 5.f, settingCenter.x + modWidth / 2 - 10.f, settingCenter.y + 25.f);
                            Vector2<float> buttonCenter = Vector2<float>((buttonRect.x + buttonRect.z) / 2, (buttonRect.y + buttonRect.w) / 2);
                            Vector4<float> extendRect = Vector4<float>(settingCenter.x - modWidth / 2 + 10.f, settingCenter.y - 5.f, settingCenter.x + modWidth / 2 - 10.f, settingCenter.y + 25.f + setting->renderEnumSlide);
                            if (setting->enumExtended) {
                                ImRenderUtils::fillRectangle(extendRect, mainBlack, 1.f, 6.f);
                                ImRenderUtils::drawRoundRect(extendRect, ImDrawFlags_None, 6.f, grayBlack, 1.f, 2.f);
                                int count = 0;
                                for (std::string& enumValue : setting->enumValues) {
                                    count++;
                                    float textSizeX = ImRenderUtils::getTextWidth(&enumValue, 1.f);
                                    float textSizeY = ImRenderUtils::getTextHeightStr(&enumValue, 1.f);
                                    Vector4<float> enumRect = Vector4<float>(buttonRect.x, buttonRect.y + modHeight * count, buttonRect.z, buttonRect.w + modHeight * count);
                                    Vector2<float> settingTextRect = Vector2<float>(buttonCenter.x - textSizeX / 2, buttonCenter.y - textSizeY / 2 + modHeight * count);
                                    ImRenderUtils::ImRenderUtils::drawText(settingTextRect, &enumValue, textColor, 1.f);

                                    if (ImRenderUtils::isMouseOver(enumRect) && isEnabled) {
                                        if (Utils::leftClick) {
                                            *setting->iterator = count - 1;
                                            Utils::leftClick = false;
                                        }
                                    }
                                }
                            }
                            ImRenderUtils::fillRectangle(buttonRect, enumBack, 1.f, 6.f);
                            ImRenderUtils::drawRoundRect(buttonRect, ImDrawFlags_None, 6.f, grayBlack, 1.f, 2.f);
                            std::string settingName = setting->getName();
                            float textSizeX = ImRenderUtils::getTextWidth(&settingName, 0.9f);
                            float textSizeY = ImRenderUtils::getTextHeightStr(&settingName, 0.9f);
                            Vector2<float> settingTextRect = Vector2<float>(buttonCenter.x - textSizeX / 2, settingCenter.y - textSizeY / 2 - 15.f);
                            ImRenderUtils::drawText(settingTextRect, &settingName, textColor, 0.9f);

                            int iterator = *(int*)setting->getIterator();
                            std::string& enumValue = setting->enumValues[iterator];
                            textSizeX = ImRenderUtils::getTextWidth(&enumValue, 1.f);
                            textSizeY = ImRenderUtils::getTextHeightStr(&enumValue, 1.f);
                            settingTextRect = Vector2<float>(buttonCenter.x - textSizeX / 2, buttonCenter.y - textSizeY / 2);
                            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[3]);
                            ImRenderUtils::ImRenderUtils::drawText(settingTextRect, &enumValue, textColor, 1.f);
                            ImGui::PopFont();

                            if (ImRenderUtils::isMouseOver(buttonRect) && isEnabled) {
                                if (Utils::leftClick) {
                                    *setting->iterator = (iterator + 1) % setting->getEnumValues().size();
                                    Utils::leftClick = false;
                                } else if (Utils::rightClick) {
                                    if (setting->enumExtended) {
                                        setting->enumExtended = false;
                                        setting->enumSlide = 0;
                                    } else {
                                        setting->enumExtended = true;
                                        setting->enumSlide = modHeight * setting->enumValues.size();
                                    }
                                    Utils::rightClick = false;
                                }
                            }
                            break;
                        }
                        case SettingType::SLIDER: {
                            float sliderHeight = 8.f;
                            float sliderPad = 8.f;
                            Vector4<float> sliderRect = Vector4<float>(settingCenter.x - modWidth / 2 + 10.f, settingCenter.y - sliderHeight / 2 + sliderPad, settingCenter.x + modWidth / 2 - 10.f, settingCenter.y + sliderHeight / 2 + sliderPad);
                            Vector2<float> sliderCenter = Vector2<float>((sliderRect.x + sliderRect.z) / 2, (sliderRect.y + sliderRect.w) / 2);

                            ImRenderUtils::fillRectangle(sliderRect, enumBack, 1.f, 20.f);
                            ImRenderUtils::drawRoundRect(sliderRect, ImDrawFlags_None, 20.f, grayBlack, 1.f, 2.f);
                            float value = *(float*)setting->getValue();
                            float round = setting->getRound();
                            float currentValue = Math::round(value, round);

                            std::ostringstream oss;
                            oss << std::fixed << std::setprecision(Math::getDecimalPlaces(round)) << (currentValue);
                            std::string str = oss.str();

                            std::string settingName = setting->getName() + ": " + str;
                            float textSizeX = ImRenderUtils::getTextWidth(&settingName, 0.9f);
                            float textSizeY = ImRenderUtils::getTextHeightStr(&settingName, 0.9f);
                            Vector2<float> settingTextRect = Vector2<float>(settingCenter.x - textSizeX / 2, settingCenter.y - textSizeY / 2 - sliderPad);
                            std::string tmpModeText = setting->getName() + ": ";
                            float tmpTextSizeX = ImRenderUtils::getTextWidth(&tmpModeText, 0.9f);
                            Vector2<float> tmpValueTextRect = Vector2<float>(settingCenter.x - textSizeX / 2 + tmpTextSizeX, settingCenter.y - textSizeY / 2 - sliderPad);
                            std::string tmpValueText = str;
                            ImRenderUtils::drawText(settingTextRect, &tmpModeText, textColor, 0.9f);
                            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[3]);
                            ImRenderUtils::drawText(tmpValueTextRect, &tmpValueText, textColor, 0.9f);
                            ImGui::PopFont();

                            float maxX = settingCenter.x + modWidth / 2 - 10.f;
                            float minX = settingCenter.x - modWidth / 2 + 10.f;
                            float deltaBar = maxX - minX;

                            float maxSetting = setting->getMax();
                            float minSetting = setting->getMin();
                            float deltaSetting = maxSetting - minSetting;

                            if (currentValue > maxSetting) currentValue = maxSetting;
                            if (currentValue < minSetting) currentValue = minSetting;

                            float deltaNowBar = currentValue - minSetting;

                            float barPerSetting = deltaNowBar / deltaSetting;
                            float barWidth = deltaBar * barPerSetting;

                            Vector4<float> fillBar = Vector4<float>(minX, settingCenter.y - sliderHeight / 2 + sliderPad, minX + barWidth, settingCenter.y + sliderHeight / 2 + sliderPad);
                            ImRenderUtils::fillRoundedGradientRectangle(fillBar, modColor, modSecColor, 20.f);

                            Vector2<float> circlePos = Vector2<float>(minX + barWidth, settingCenter.y + sliderPad);
                            ImRenderUtils::fillCircle(circlePos, 7.f, mainBlack, 1.f, 360.f);
                            ImRenderUtils::drawCircleOutline(circlePos, 7.f, modSecColor, 1.f, 360.f, 2.f);

                            if (ImRenderUtils::isMouseOver(sliderRect) && isEnabled && Utils::leftClick) setting->isDragging = true;

                            if (Utils::leftClick && setting->isDragging && isEnabled) {
                                float mouseX = ImRenderUtils::getMousePos().x;
                                float addVal = mouseX - minX;
                                if (addVal + minX > maxX) addVal = deltaBar;
                                if (addVal < 0) addVal = 0;
                                float idk = addVal / deltaBar;
                                *(float*)setting->value = Math::round(idk * deltaSetting + minSetting, round);
                            }
                            else setting->isDragging = false;
                            break;
                        }
                        }
                        settingsHeight += settingHeight;
                    }
                    ImClippingUtil::restoreClipping();
                }
                mod->settingsScale = Math::animate(settingsHeight, mod->settingsScale, ImRenderUtils::getDeltaTime() * 10);
                myHeight += mod->settingsScale;
                moduleY += myHeight;
            }

            //Render Category tabs
            ImRenderUtils::fillRectangle(catTabRect, darkBlack, 1.f, 12, ImDrawFlags_RoundCornersTop);
            std::string catName = categories[i];
            ImGui::PopFont();
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[3]);
            ImRenderUtils::drawText(catNameRect, &catName, textColor, 1.3f);
            ImGui::PopFont();

            catPositions[i].shadowRect = Vector4<float>(catPositions[i].x, catPositions[i].y, catPositions[i].x + catWidth, catPositions[i].y + catHeight + moduleY);
        }
    }
    if (isEnabled) {
        Utils::leftDown = false;
        Utils::rightDown = false;
        Utils::rightClick = false;
        Address::getClientInstance()->grabMouse();
        scrollDirection = 0;
    }
}
*/
