//
// Created by vastrakai on 6/29/2024.
//

#include "ImRenderUtils.hpp"

#include "D2D.hpp"

void ImRenderUtils::addBlur(const ImVec4& pos, float strength, float radius)
{
    if (!ImGui::GetCurrentContext())
        return;

    D2D::addBlur(ImGui::GetForegroundDrawList(), strength, pos, radius);
}
