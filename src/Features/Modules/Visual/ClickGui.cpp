//
// Created by vastrakai on 6/29/2024.
//

#include "ClickGui.hpp"

#include <imgui.h>
#include <Features/Events/MouseEvent.hpp>
#include <Features/Events/KeyEvent.hpp>
#include <Features/GUI/Dropdown.hpp>
#include <Utils/MiscUtils/ImRenderUtils.hpp>
#include <Utils/MiscUtils/MathUtils.hpp>
#include <Utils/MiscUtils/EasingUtil.hpp>

static bool lastMouseState = false;
static DropdownGui dropdownGui = DropdownGui();


void ClickGui::onEnable()
{
    auto ci = ClientInstance::get();
    lastMouseState = !ci->getMouseGrabbed();

    ci->releaseMouse();

    gFeatureManager->mDispatcher->listen<MouseEvent, &ClickGui::onMouseEvent>(this);
    gFeatureManager->mDispatcher->listen<KeyEvent, &ClickGui::onKeyEvent>(this);
}

void ClickGui::onDisable()
{
    gFeatureManager->mDispatcher->deafen<MouseEvent, &ClickGui::onMouseEvent>(this);
    gFeatureManager->mDispatcher->deafen<KeyEvent, &ClickGui::onKeyEvent>(this);

    if (lastMouseState) {
        ClientInstance::get()->grabMouse();
    }
}


void ClickGui::onMouseEvent(MouseEvent& event)
{
    event.mCancelled = true;
}

void ClickGui::onKeyEvent(KeyEvent& event)
{
    if (event.mKey == VK_ESCAPE) {
        if (!dropdownGui.isBinding && event.mPressed) this->toggle();
        event.mCancelled = true;
    }
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
