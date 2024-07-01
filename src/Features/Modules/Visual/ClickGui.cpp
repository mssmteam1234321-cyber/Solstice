//
// Created by vastrakai on 6/29/2024.
//

#include "ClickGui.hpp"

#include <imgui.h>
#include <Features/GUI/Dropdown.hpp>
#include <Utils/MiscUtils/ImRenderUtils.hpp>
#include <Utils/MiscUtils/MathUtils.hpp>
#include <Utils/MiscUtils/EasingUtil.hpp>

void ClickGui::onEnable()
{

}

void ClickGui::onDisable()
{

}



void ClickGui::onRenderEvent(RenderEvent& event)
{
    static float animation = 0;
    static int animationMode = 0; // Ease enum
    static bool blur = true;
    static int styleMode = 0; // Ease enum
    static float animationSpeed = 10.5f; // Ease speed
    static int scrollDirection = 0;
    static char h[2] = { 0 };
    static DropdownGui dropdownGui = DropdownGui();
    static EasingUtil inEase = EasingUtil();

    float delta = ImGui::GetIO().DeltaTime;

    this->mEnabled ? inEase.incrementPercentage(delta * animationSpeed / 10)
    : inEase.decrementPercentage(delta * 2 * animationSpeed / 10);
    float inScale = inEase.easeOutExpo();
    if (inEase.isPercentageMax()) inScale = 1;
    animation = MathUtils::lerp(0, 1, inEase.easeOutExpo());

    if (animation < 0.0001f) {
        return;
    }

    dropdownGui.render(animation, inScale, scrollDirection, h, blur);
}
