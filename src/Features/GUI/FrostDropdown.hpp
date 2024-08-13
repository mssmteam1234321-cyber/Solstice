//
// Created by Tozic on 8/7/2024.
//

#pragma once
#include <vector>
#include <Features/FeatureManager.hpp>
#include <Features/Modules/Setting.hpp>



#include "glm/glm.hpp"

class Module;

class FrostGui
{
public:
    struct CategoryPosition
    {
        float x = 0.f, y = 0.f;
        bool isDragging = false, isExtended = true, wasExtended = false;
        float yOffset = 0;
        float scrollEase = 0;
        glm::vec2 dragVelocity = glm::vec2(0, 0);
        glm::vec2 dragAcceleration = glm::vec2(0, 0);
    };

    const float catWidth = 193.f;
    const float catHeight = 32.5f;
    const float colorPickerHeight = 90;

    ImColor moduleBackgroundColor = ImColor(40, 40, 40);
    ImColor settingsBackgroundColor = ImColor(24, 24, 24);
    ImColor categoryHeaderColor = ImColor(20, 20, 20);

    const float catGap = 40;
    int lastDragged = -1;
    std::vector<CategoryPosition> catPositions;
    std::shared_ptr<Module> lastMod = nullptr;
    bool isBinding = false;
    ColorSetting* lastColorSetting = nullptr;
    bool displayColorPicker = false;
    bool resetPosition = false;
    uint64_t lastReset = 0;

    // scaleToPoint
    ImVec4 scaleToPoint(const ImVec4& _this, const ImVec4& point, float amount);
    bool isMouseOver(const ImVec4& rect);
    ImVec4 getCenter(ImVec4& vec);
    bool isMouseOverGuiElement();
    void render(float animation, float inScale, int& scrollDirection, char* h, float blur, float midclickRounding);
    void onWindowResizeEvent(class WindowResizeEvent& event);
};
