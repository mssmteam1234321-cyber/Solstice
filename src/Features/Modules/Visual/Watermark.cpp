//
// Created by vastrakai on 6/29/2024.
//

#include "Watermark.hpp"

#include <Hook/Hooks/RenderHooks/D3DHook.hpp>

void Watermark::onEnable()
{
}

void Watermark::onDisable()
{
}

void Watermark::onRenderEvent(RenderEvent& event)
{
    static float anim = 0.f;
    anim = MathUtils::lerp(anim, mEnabled ? 1.f : 0.f, ImGui::GetIO().DeltaTime * 10.f);
    anim = MathUtils::clamp(anim, 0.f, 1.f);

    if (anim < 0.01f) return;

    int initialPos = -50;
    static auto renderPosition = ImVec2(initialPos, initialPos);


    if (mStyle.mValue == Style::SevenDays)
    {
        renderPosition.x = MathUtils::lerp(-200, 20.f, anim);
        renderPosition.y = MathUtils::lerp(-200, 20.f, anim);
        static std::string filePath = "seven_days.png";
        static ID3D11ShaderResourceView* texture;
        static bool loaded = false;
        static int width, height;
        if (!loaded)
        {
            D3DHook::loadTextureFromEmbeddedResource(filePath.c_str(), &texture, &width, &height);
            loaded = true;
            width /= 10;
            height /= 10;
        }

        // Get the exact center-point of the image
        ImVec2 centeredImgPos = ImVec2(renderPosition.x + width / 2, renderPosition.y + height / 2);

        // Add a red shadow circle
        if (mGlow.mValue)
            ImGui::GetBackgroundDrawList()->AddShadowCircle(centeredImgPos, (width / 2) * 1.05f, ImColor(1.f, 0.f, 0.f, anim), 100, ImVec2(0.f, 0.f), 0, 100);

        ImGui::GetBackgroundDrawList()->AddImage(texture, renderPosition, ImVec2(renderPosition.x + width, renderPosition.y + height));
        return;
    }
    renderPosition.x = MathUtils::lerp(initialPos, 20.f, anim);
    renderPosition.y = MathUtils::lerp(initialPos, 20.f, anim);

    FontHelper::pushPrefFont(true, mBold.mValue);

    static std::string watermarkText = "solstice";
    static float size = 45;




    for (int i = 0; i < watermarkText.length(); i++)
    {
        // get the character
        char c = watermarkText[i];
        // get the width of the character
        ImVec2 charSize = ImGui::GetFont()->CalcTextSizeA(size, FLT_MAX, 0.0f, &c, &c + 1);
        float charWidth = charSize.x;

        ImColor color = ColorUtils::getThemedColor(i * 100);

        if (mGlow.mValue)
            ImGui::GetBackgroundDrawList()->AddShadowCircle(ImVec2(renderPosition.x + (charSize.x / 2), renderPosition.y + (charSize.y / 2)),
                                                            size / 3, ImColor(color.Value.x, color.Value.y, color.Value.z, anim * 0.75f), 100, ImVec2(0.f, 0.f), 0, 12);
        // draw a shadow
        ImColor shadowColor = ImColor(color.Value.x * 0.25f, color.Value.y * 0.25f, color.Value.z * 0.25f, 0.925f);
        ImVec2 shadowPos = renderPosition;

        shadowPos.x += charSize.x / 10 - 0.5f;
        shadowPos.y += charSize.y / 10 - 0.5f;
        ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, shadowPos, shadowColor, &c, &c + 1);

        // draw the character
        ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, renderPosition, color, &c, &c + 1);
        // move the render position to the right
        renderPosition.x += charWidth;
    }

    ImGui::PopFont();
}
