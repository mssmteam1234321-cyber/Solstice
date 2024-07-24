//
// Created by vastrakai on 6/29/2024.
//

#include "FontHelper.hpp"

#include <Features/Modules/Visual/Interface.hpp>

#include "Resources.hpp"

void FontHelper::load()
{
    if (ImGui::GetFont()) return; // Already loaded
    ResourceLoader::loadResources();
}

void FontHelper::pushPrefFont(bool large, bool bold)
{
    auto font = getFont(large, bold);
    if (!font) return;
    ImGui::PushFont(font);
}

ImFont* FontHelper::getFont(bool large, bool bold)
{
    static auto daInterface = gFeatureManager->mModuleManager->getModule<Interface>();
    if (!daInterface) return nullptr;

    auto fontSel = daInterface->mFont.as<Interface::FontType>();
    // kinda goofy but it will work for now :3
    // each font here should have a font_large, font_bold, and font_bold_large
    if (fontSel == Interface::FontType::Mojangles)
        return Fonts[large ? (bold ? "mojangles_bold_large" : "mojangles_large") : (bold ? "mojangles_bold" : "mojangles")];
    if (fontSel == Interface::FontType::ProductSans)
        return Fonts[large ? (bold ? "product_sans_bold_large" : "product_sans_large") : (bold ? "product_sans_bold" : "product_sans")];
    if (fontSel == Interface::FontType::Comfortaa)
        return Fonts[large ? (bold ? "comfortaa_bold_large" : "comfortaa_large") : (bold ? "comfortaa_bold" : "comfortaa")];
    return nullptr;
}
