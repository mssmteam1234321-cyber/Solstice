//
// Created by vastrakai on 7/7/2024.
//

#include "RenderUtils.hpp"

#include "ColorUtils.hpp"
#include "MathUtils.hpp"

void RenderUtils::drawOutlinedAABB(const AABB& aabb, bool filled)
{
    std::vector<ImVec2> imPoints = MathUtils::getImBoxPoints(aabb);

    auto drawList = ImGui::GetBackgroundDrawList();

    ImColor themeColor = ColorUtils::getThemedColor(0);

    if (filled) drawList->AddConvexPolyFilled(imPoints.data(), imPoints.size(), ImColor(themeColor.Value.x, themeColor.Value.y, themeColor.Value.z, 0.25f));
    drawList->AddPolyline(imPoints.data(), imPoints.size(), themeColor, 0, 2.0f);
}
