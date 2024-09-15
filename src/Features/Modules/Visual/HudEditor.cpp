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
    try
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

            spdlog::info("Saved element: {}", element->mParentTypeIdentifier);
        }

        for (auto& element : mCustomElements)
        {
            j[element->mParentTypeIdentifier] = {
                {"pos", {element->mPos.x, element->mPos.y}},
                {"size", {element->mSize.x, element->mSize.y}},
                {"anchor", element->mAnchor},
                {"text", element->mText},
                {"fontSize", element->mFontSize}
            };

            spdlog::info("Saved custom element: {}", element->mText);
        }

        j["snapDistance"] = mSnapDistance;

        std::ofstream file(path);
        file << j.dump(4);
        file.close();

        spdlog::info("Saved hud elements to file!");
    } catch (const std::exception& e)
    {
        spdlog::error("Failed to save hud elements to file: {}", e.what());
    } catch (const nlohmann::json::exception& e)
    {
        spdlog::error("Failed to save hud elements to file: {}", e.what());
    } catch (...)
    {
        spdlog::error("Failed to save hud elements to file: unknown error");
    }
}

void HudEditor::loadFromFile()
{
    try
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

                // remove the element from the json
                j.erase(element->mParentTypeIdentifier);
            }


        }

        if (j.contains("snapDistance"))
        {
            mSnapDistance = j["snapDistance"];
            j.erase("snapDistance");
        }

        // For the rest of the keys, they are custom elements
        for (auto& [key, value] : j.items())
        {
            auto element = std::make_unique<CustomHudElement>(key.c_str(), value["text"], CustomHudElement::Type::Text, value["text"]);
            element->mPos = { value["pos"][0], value["pos"][1] };
            element->mSize = { value["size"][0], value["size"][1] };
            element->mAnchor = value["anchor"];
            element->mFontSize = value["fontSize"];
            // Copy the mText to the input buffer
            strcpy(element->mInputBuffer, element->mText.c_str());

            // if the key is not in the elements, add it to the custom elements
            bool found = false;
            for (auto& element : mCustomElements)
            {
                if (element->mParentTypeIdentifier == key)
                {
                    element->mPos = { value["pos"][0], value["pos"][1] };
                    element->mSize = { value["size"][0], value["size"][1] };
                    element->mAnchor = value["anchor"];
                    element->mFontSize = value["fontSize"];
                    element->mText = value["text"];
                    element->mVisible = true;
                    // Copy the mText to the input buffer
                    strcpy(element->mInputBuffer, element->mText.c_str());
                    found = true;
                }
            }

            if (!found)
                mCustomElements.push_back(std::move(element));
        }

        spdlog::info("Loaded hud elements from file!");
    } catch (const std::exception& e)
    {
        spdlog::error("Failed to load hud elements from file: {}", e.what());
    } catch (const nlohmann::json::exception& e)
    {
        spdlog::error("Failed to load hud elements from file: {}", e.what());
        // Save the file to prevent further errors (Exceptions here are usually caused by missing elements)
        saveToFile();
    } catch (...)
    {
        spdlog::error("Failed to load hud elements from file: unknown error");
    }
}

void HudEditor::onInit()
{
    loadFromFile();
}

void HudEditor::onEnable()
{
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
    if (!mEnabled) return;

    // Draw grid lines
    auto drawList = ImGui::GetBackgroundDrawList();
    ImVec2 display = ImGui::GetIO().DisplaySize;

    auto ci = ClientInstance::get();

    ci->releaseMouse();

    // Draw background
    drawList->AddRectFilled(ImVec2(0, 0), display, IM_COL32(0, 0, 0, 50));

    const int snapDistance = mSnapDistance; // Copy to prevent race conditions


    static bool dragging = false;
    static ImVec2 dragStart;
    static ImVec2 dragOffset;
    static HudElement* draggedElement = nullptr;
    static bool guiOpen = false;


    if (snapDistance > 0 && !guiOpen)
    {
        for (int i = 0; i < display.x; i += snapDistance)
            drawList->AddLine(ImVec2(i, 0), ImVec2(i, display.y), IM_COL32(255, 255, 255, 100));
        for (int i = 0; i < display.y; i += snapDistance)
            drawList->AddLine(ImVec2(0, i), ImVec2(display.x, i), IM_COL32(255, 255, 255, 100));
    }


    if (ImGui::IsMouseClicked(0) && !guiOpen)
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

        for (auto& element : mCustomElements)
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
                draggedElement = element.get();
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

    for (auto& element : mCustomElements)
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

    // Draw elements
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

    for (auto& element : mCustomElements)
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

    // Draw a circle with a + in the bottom left (add new element button)
    float radius = 15;
    ImVec2 buttonPos = { 30, display.y - 30 };
    ImVec4 buttonRect = { buttonPos.x - radius, buttonPos.y - radius, buttonPos.x + radius, buttonPos.y + radius };

    static ImColor butColor = ImColor(255, 255, 255, 125);
    ImColor targetColor = guiOpen ? ImColor(0, 255, 0, 200) : ImRenderUtils::isMouseOver(buttonRect) ? ImColor(255, 255, 255, 255) : ImColor(255, 255, 255, 125);
    butColor.Value = MathUtils::lerp(butColor.Value, targetColor.Value, ImGui::GetIO().DeltaTime * 10);

    drawList->AddCircleFilled(buttonPos, radius, butColor);
    drawList->AddLine(ImVec2(buttonRect.x + 5, buttonRect.y + radius), ImVec2(buttonRect.z - 5, buttonRect.y + radius), IM_COL32(0, 0, 0, 255), 2);
    drawList->AddLine(ImVec2(buttonRect.x + radius, buttonRect.y + 5), ImVec2(buttonRect.x + radius, buttonRect.w - 5), IM_COL32(0, 0, 0, 255), 2);


    if (ImGui::IsMouseClicked(0)) {
        if (ImRenderUtils::isMouseOver(buttonRect)) {
            guiOpen = !guiOpen;
        }
    }

    if (!guiOpen) return;

    // Draw in center
    ImVec2 center = { display.x / 2, display.y / 2 };
    ImVec2 size = { 400, 400 };
    ImVec2 pos = { center.x - size.x / 2, center.y - size.y / 2 };

    ImGui::SetNextWindowPos(pos);
    ImGui::SetNextWindowSize(size);
    ImGui::Begin("Hud Editor", &guiOpen, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);

    ImGui::Text("Hello, world!");

    // Add text button
    if (ImGui::Button("Add Text"))
    {
        auto element = std::make_unique<CustomHudElement>(StringUtils::randomString(32).c_str(), "Sample Text", CustomHudElement::Type::Text, "Sample Text");
        element->mVisible = true;
        // Copy the mText to the input buffer
        strcpy(element->mInputBuffer, element->mText.c_str());

        mCustomElements.push_back(std::move(element));
        NotifyUtils::notify("Added new text element!", 1.f, Notification::Type::Info);
    }

    for (int i = 0; i < mCustomElements.size(); i++)
    {
        auto& element = mCustomElements[i];
        if (!element->mVisible) continue;

        ImGui::InputText("Text", element->mInputBuffer, 256);
        ImGui::SameLine();
        ImGui::InputFloat("Font Size", &element->mFontSize);

        if (ImGui::Button("Delete"))
        {
            NotifyUtils::notify("Deleted element!", 1.f, Notification::Type::Info);

            mCustomElements.erase(mCustomElements.begin() + i);
            i--;
        }

        ImGui::SameLine();

        if (ImGui::Button("Set"))
        {
            element->mText = element->mInputBuffer;
            NotifyUtils::notify("Set text!", 1.f, Notification::Type::Info);
        }

        ImGui::SameLine();

        if (ImGui::Button("Clear"))
        {
            element->mText = "";
            strcpy(element->mInputBuffer, "");
            NotifyUtils::notify("Cleared text!", 1.f, Notification::Type::Info);
        }
    }

    ImGui::End();

}

void HudEditor::onCustomRenderEvent(RenderEvent& event)
{
    // TODO: Render custom elements here...


    for (auto& element : mCustomElements)
    {
        FontHelper::pushPrefFont(element->mFontSize > 30);

        ImVec2 pos = element->getPos();

        auto drawList = ImGui::GetBackgroundDrawList();

        drawList->AddText(ImGui::GetFont(), element->mFontSize, pos, IM_COL32(255, 255, 255, 255), element->mText.c_str());
        ImVec2 size = ImGui::GetFont()->CalcTextSizeA(element->mFontSize, FLT_MAX, 0, element->mText.c_str());
        element->mSize = { size.x, size.y };

        FontHelper::popPrefFont();

    }

}

bool mShiftHeld = false;

void HudEditor::onKeyEvent(KeyEvent& event)
{
    // If imgui is capturing the keyboard, don't do anything
    if (ImGui::GetIO().WantCaptureKeyboard)
    {
        return;
    }

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