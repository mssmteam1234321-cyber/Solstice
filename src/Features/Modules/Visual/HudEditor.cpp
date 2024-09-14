//
// Created by vastrakai on 9/14/2024.
//

#include "HudEditor.hpp"

#include <Features/Events/KeyEvent.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/MinecraftGame.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>

#include "ClickGui.hpp"

bool lastCscState = false;

void HudEditor::showAllElements()
{
    for (auto element : mElements) {
        element->mSampleMode = true;
    }
}

void HudEditor::hideAllElements()
{
    for (auto element : mElements) {
        element->mSampleMode = false;
    }
}

void HudEditor::saveToFile()
{
    static std::string path = FileUtils::getSolsticeDir() + "hud.json";
    nlohmann::json j;
    for (auto element : mElements)
    {
        j[element->mParentTypeIdentifier] = {
            {"pos", {element->mPos.x, element->mPos.y}},
            {"size", {element->mSize.x, element->mSize.y}},
            {"anchor", element->mAnchor}
        };
    }
    j["snapDistance"] = mSnapDistance;
    std::ofstream file(path);
    file << j.dump(4);
    file.close();

    spdlog::info("Saved hud elements to file!");
}

void HudEditor::loadFromFile()
{
    static std::string path = FileUtils::getSolsticeDir() + "hud.json";
    if (!FileUtils::fileExists(path))
    {
        spdlog::warn("No hud elements file found, creating one!");
        saveToFile();
        return;
    }
    nlohmann::json j;
    std::ifstream file(path);
    file >> j;
    file.close();

    for (auto element : mElements)
    {
        if (j.contains(element->mParentTypeIdentifier))
        {
            auto& data = j[element->mParentTypeIdentifier];
            element->mPos = { data["pos"][0], data["pos"][1] };
            element->mSize = { data["size"][0], data["size"][1] };
            element->mAnchor = data["anchor"];
        }

        if (j.contains("snapDistance"))
        {
            mSnapDistance = j["snapDistance"];
        }
    }

    spdlog::info("Loaded hud elements from file!");
}

void HudEditor::onInit()
{
    loadFromFile();
}

void HudEditor::onEnable()
{
    gFeatureManager->mDispatcher->listen<RenderEvent, &HudEditor::onRenderEvent, nes::event_priority::FIRST>(this);
    gFeatureManager->mDispatcher->listen<KeyEvent, &HudEditor::onKeyEvent>(this);
    gFeatureManager->mDispatcher->listen<MouseEvent, &HudEditor::onMouseEvent>(this);

    auto ci = ClientInstance::get();
    lastCscState = !ci->getMouseGrabbed();
    ci->releaseMouse();

    loadFromFile();
    showAllElements();

    static auto clickGuiModule = gFeatureManager->mModuleManager->getModule<ClickGui>();
    if (clickGuiModule) clickGuiModule->setEnabled(false);
}

void HudEditor::onDisable()
{
    gFeatureManager->mDispatcher->deafen<RenderEvent, &HudEditor::onRenderEvent>(this);
    gFeatureManager->mDispatcher->deafen<KeyEvent, &HudEditor::onKeyEvent>(this);
    gFeatureManager->mDispatcher->deafen<MouseEvent, &HudEditor::onMouseEvent>(this);

    auto ci = ClientInstance::get();
    if (lastCscState) ci->grabMouse();
    else ci->releaseMouse();

    saveToFile();
    hideAllElements();
}

void HudEditor::onRenderEvent(RenderEvent& event)
{
    // Draw grid lines
    auto drawList = ImGui::GetBackgroundDrawList();
    ImVec2 display = ImGui::GetIO().DisplaySize;

    auto ci = ClientInstance::get();

    ci->releaseMouse();

    // Draw background
    drawList->AddRectFilled(ImVec2(0, 0), display, IM_COL32(0, 0, 0, 50));

    if (mSnapDistance != 0)
    {
        for (int i = 0; i < display.x; i += mSnapDistance)
            drawList->AddLine(ImVec2(i, 0), ImVec2(i, display.y), IM_COL32(255, 255, 255, 100));
        for (int i = 0; i < display.y; i += mSnapDistance)
            drawList->AddLine(ImVec2(0, i), ImVec2(display.x, i), IM_COL32(255, 255, 255, 100));
    }

    for (auto element : mElements)
    {
        if (!element->mVisible) continue;

        ImVec2 pos = element->getPos();
        ImVec2 size = element->mSize;
        if (element->mCentered)
        {
            pos.x -= size.x / 2;
            pos.y -= size.y / 2;
        }

        drawList->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), IM_COL32(255, 255, 255, 125));
    }

    static bool dragging = false;
    static ImVec2 dragStart;
    static ImVec2 dragOffset;
    static HudElement* draggedElement = nullptr;

    if (ImGui::IsMouseClicked(0))
    {
        dragStart = ImGui::GetMousePos();
        dragOffset = { 0, 0 };

        for (auto element : mElements)
        {
            if (!element->mVisible) continue;

            ImVec2 pos = element->getPos();
            ImVec2 size = element->mSize;

            if (element->mCentered)
            {
                pos.x -= size.x / 2;
                pos.y -= size.y / 2;
            }

            if (dragStart.x > pos.x && dragStart.x < pos.x + size.x &&
                dragStart.y > pos.y && dragStart.y < pos.y + size.y)
            {
                dragging = true;
                draggedElement = element;
                dragOffset = { dragStart.x - pos.x, dragStart.y - pos.y };
                break;
            }
        }
    }

    if (dragging && draggedElement != nullptr)
    {
        ImVec2 dragEnd = ImGui::GetMousePos();

        glm::ivec2 delta = { dragEnd.x - dragStart.x, dragEnd.y - dragStart.y };
        glm::ivec2 newPos = { dragEnd.x - dragOffset.x, dragEnd.y - dragOffset.y };

        if (mSnapDistance != 0)
        {
            newPos.x = std::round(newPos.x / mSnapDistance) * mSnapDistance;
            newPos.y = std::round(newPos.y / mSnapDistance) * mSnapDistance;
        }

        if (draggedElement->mCentered)
        {
            newPos.x += draggedElement->mSize.x / 2.f;
            newPos.y += draggedElement->mSize.y / 2.f;
        }

        draggedElement->setFromPos(newPos);

        if (!ImGui::IsMouseDown(0))
        {
            dragging = false;
            draggedElement = nullptr;
        }
    }

    // Loop through all of the positions and clamp them to the screen, including the size+pos
    for (auto element : mElements)
    {
        if (!element->mVisible) continue;

        auto pos = element->getPos();
        glm::vec2 size = element->mSize;

        if (element->mCentered)
        {
            pos.x -= size.x / 2;
            pos.y -= size.y / 2;
        }

        pos.x = std::clamp(pos.x, 0.f, display.x - element->mSize.x);
        pos.y = std::clamp(pos.y, 0.f, display.y - element->mSize.y);
        if (element->mSize.x + pos.x > display.x) element->mSize.x = display.x - pos.x;
        if (element->mSize.y + pos.y > display.y) element->mSize.y = display.y - pos.y;

        auto newPos = pos;

        if (element->mCentered)
        {
            newPos.x += size.x / 2;
            newPos.y += size.y / 2;
        }

        element->setFromPos(glm::vec2(newPos.x, newPos.y));
    }
}

bool mShiftHeld = false;

void HudEditor::onKeyEvent(KeyEvent& event)
{
    if (event.mKey == mKey && event.mPressed)
    {
        setEnabled(false); // ????
        return;
    }

    if (event.mKey == VK_SHIFT)
    {
        mShiftHeld = event.mPressed;
        event.cancel();
        return;
    }

    if (event.mKey == VK_TAB)
    {
        event.cancel();

        if (event.mPressed)
        {
            if (mShiftHeld) mSnapDistance -= 5;
            else mSnapDistance += 5;

            if (mSnapDistance > 100) mSnapDistance = 0;
            if (mSnapDistance < 0) mSnapDistance = 100;

            NotifyUtils::notify("Snap distance: " + std::to_string(mSnapDistance), 2.f, Notification::Type::Info);
        }
    }

    if (event.mKey == VK_ESCAPE)
    {
        event.cancel();
        setEnabled(false);
    }


}

void HudEditor::onMouseEvent(MouseEvent& event)
{
    event.cancel();
}