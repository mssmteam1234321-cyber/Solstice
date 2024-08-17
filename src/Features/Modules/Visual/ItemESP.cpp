//
// Created by vastrakai on 8/7/2024.
//

#include "ItemESP.hpp"

#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Actor/ItemActor.hpp>
#include <SDK/Minecraft/Inventory/Item.hpp>
#include <SDK/Minecraft/Rendering/GuiData.hpp>

void ItemESP::onEnable()
{
    gFeatureManager->mDispatcher->listen<RenderEvent, &ItemESP::onRenderEvent>(this);
}

void ItemESP::onDisable()
{
    gFeatureManager->mDispatcher->deafen<RenderEvent, &ItemESP::onRenderEvent>(this);
}

void ItemESP::onRenderEvent(RenderEvent& event)
{
    if (!ClientInstance::get()->getLevelRenderer()) return;

    auto ci = ClientInstance::get();
    auto player = ci->getLocalPlayer();
    if (!player) return;
    if (!ci->getLevelRenderer()) return;

    auto drawList = ImGui::GetBackgroundDrawList();
    auto actors = ActorUtils::getActorsTyped<ItemActor>(ActorType::ItemEntity);

    for (auto actor : actors)
    {
        if (!actor) continue;
        if (!actor->mItem.mItem) continue;
        if (!actor->getStateVectorComponent()) continue;
        if (mDistanceLimited.mValue && player->distanceTo(actor) > mDistance.mValue) continue;

        AABB aabb = actor->getAABB();
        std::vector<ImVec2> imPoints = MathUtils::getImBoxPoints(aabb);

        ImColor themeColor = mThemedColor.mValue ? ColorUtils::getThemedColor(0) : ImColor(1.0f, 1.0f, 1.0f);

        if (mRenderFilled.mValue) drawList->AddConvexPolyFilled(imPoints.data(), imPoints.size(), ImColor(themeColor.Value.x, themeColor.Value.y, themeColor.Value.z, 0.25f));
        drawList->AddPolyline(imPoints.data(), imPoints.size(), themeColor, 0, 2.0f);

        auto renderPosComp = actor->getRenderPositionComponent();
        if (!renderPosComp) continue;

        glm::vec3 pos = renderPosComp->mPosition;
        glm::vec3 origin = RenderUtils::transform.mOrigin;
        glm::vec2 screen = glm::vec2(0, 0);


        if (!RenderUtils::transform.mMatrix.OWorldToScreen(origin, pos, screen, ci->getFov(), ci->getGuiData()->mResolution)) continue;

        if (!mShowNames.mValue) continue;
        if (!actor->mItem.mItem) continue;

        ItemStack* stack = &actor->mItem;

        if (!stack->mItem) continue;

        std::string name = actor->mItem.getItem()->mName;
        if (name.empty()) return;
        name += " x" + std::to_string(stack->mCount);
        FontHelper::pushPrefFont(true, true);

        float fontSize = mFontSize.mValue;

        if (mDistanceScaledFont.mValue)
        {
            // use distance to origin, not actor
            float distance = glm::distance(origin, pos) + 2.5f;
            if (distance < 0) distance = 0;
            fontSize = 1.0f / distance * 100.0f * mScalingMultiplier.mValue;
            if (fontSize < 1.0f) fontSize = 1.0f;
        }

        ImVec2 textSize = ImGui::GetFont()->CalcTextSizeA(fontSize, FLT_MAX, 0, name.c_str());
        ImVec2 textPos = ImVec2(screen.x - textSize.x / 2, screen.y - textSize.y - 5);
        //drawList->AddText(ImGui::GetFont(), fontSize, textPos, ImColor(1.0f, 1.0f, 1.0f, 1.0f), name.c_str());
        ImColor textCol = ImColor(1.0f, 1.0f, 1.0f, 1.0f);

        ImRenderUtils::drawShadowText(drawList, name, textPos, textCol, fontSize, true);

        FontHelper::popPrefFont();


    }
}
