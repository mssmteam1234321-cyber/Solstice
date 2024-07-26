//
// Created by vastrakai on 7/7/2024.
//

#include "RenderUtils.hpp"

#include "ColorUtils.hpp"
#include "MathUtils.hpp"

void RenderUtils::drawOutlinedAABB(const AABB& aabb, bool filled, const ImColor& color)
{
    std::vector<ImVec2> imPoints = MathUtils::getImBoxPoints(aabb);

    auto drawList = ImGui::GetBackgroundDrawList();

    if (filled) drawList->AddConvexPolyFilled(imPoints.data(), imPoints.size(), ImColor(color.Value.x, color.Value.y, color.Value.z, 0.25f));
    drawList->AddPolyline(imPoints.data(), imPoints.size(), color, 0, 2.0f);
}
