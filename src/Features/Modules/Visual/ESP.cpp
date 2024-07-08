//
// Created by vastrakai on 7/7/2024.
//

#include "ESP.hpp"

#include <Features/FeatureManager.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <Utils/Structs.hpp>
#include <Utils/GameUtils/ActorUtils.hpp>
#include <Utils/MiscUtils/ColorUtils.hpp>
#include <Utils/MiscUtils/MathUtils.hpp>

void ESP::onEnable()
{
    gFeatureManager->mDispatcher->listen<RenderEvent, &ESP::onRenderEvent>(this);
}

void ESP::onDisable()
{
    gFeatureManager->mDispatcher->deafen<RenderEvent, &ESP::onRenderEvent>(this);
}

void ESP::onRenderEvent(RenderEvent& event)
{
    auto actors = ActorUtils::getActorList(false);

    auto drawList = ImGui::GetBackgroundDrawList();

    for (auto actor : actors)
    {
        auto shape = actor->getAABBShapeComponent();
        if (!shape) continue;
        AABB aabb = actor->getAABB();

        glm::vec3 min = aabb.mMin;
        glm::vec3 max = aabb.mMax;
        std::vector<glm::vec2> points = MathUtils::getBoxPoints(aabb);
        std::vector<ImVec2> imPoints = {};
        for (auto point : points)
        {
            imPoints.push_back(ImVec2(point.x, point.y));
        }

        // try drawing as convex polygon
        auto themeColor = ColorUtils::getThemedColor(0);

        drawList->AddConvexPolyFilled(imPoints.data(), points.size(), ImColor(themeColor.Value.x, themeColor.Value.y, themeColor.Value.z, 0.25f));
        drawList->AddPolyline(imPoints.data(), points.size(), themeColor, 0, 2.0f);
    }
}
