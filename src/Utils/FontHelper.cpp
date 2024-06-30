//
// Created by vastrakai on 6/29/2024.
//

#include "FontHelper.hpp"
#include "Resources.hpp"

void FontHelper::load()
{
    if (ImGui::GetFont()) return; // Already loaded
    ResourceLoader::loadResources();
}
