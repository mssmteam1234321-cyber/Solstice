#pragma once
#include <vector>
#include <Features/FeatureManager.hpp>
#include <Features/Modules/Setting.hpp>



#include "glm/glm.hpp"
//
// Created by vastrakai on 7/1/2024.
//


class Module;

class DropdownGui
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

    const float catWidth = 185.f;
    const float catHeight = 28.5f;

    const float catGap = 40;
    int lastDragged = -1;
    std::vector<CategoryPosition> catPositions;
    std::shared_ptr<Module> lastMod = nullptr;
    bool isBinding = false;
    ColorSetting* lastColorSetting = nullptr;
    bool displayColorPicker = false;

    // scaleToPoint
    ImVec4 scaleToPoint(const ImVec4& _this, const ImVec4& point, float amount);
    bool isMouseOver(const ImVec4& rect);
    ImVec4 getCenter(ImVec4& vec);
    bool isMouseOverGuiElement();
    void render(float animation, float inScale, int& scrollDirection, char* h, float blur, float midclickRounding);
    void onWindowResizeEvent(class WindowResizeEvent& event);
};
