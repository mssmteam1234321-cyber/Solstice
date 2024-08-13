//
// Created by vastrakai on 8/10/2024.
//

#include "Nametags.hpp"

#include <Features/Events/CanShowNameTagEvent.hpp>
#include <Features/Modules/Misc/Friends.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Options.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/Actor/Components/FlagComponent.hpp>
#include <SDK/Minecraft/Rendering/GuiData.hpp>

void Nametags::onEnable()
{
    gFeatureManager->mDispatcher->listen<CanShowNameTagEvent, &Nametags::onCanShowNameTag>(this);
    gFeatureManager->mDispatcher->listen<RenderEvent, &Nametags::onRenderEvent>(this);
}
void Nametags::onDisable()
{
    gFeatureManager->mDispatcher->deafen<CanShowNameTagEvent, &Nametags::onCanShowNameTag>(this);
    gFeatureManager->mDispatcher->deafen<RenderEvent, &Nametags::onRenderEvent>(this);
}

void Nametags::onCanShowNameTag(CanShowNameTagEvent& event)
{
    auto actor = event.mActor;
    if (!actor->isPlayer()) return;
    if (actor == ClientInstance::get()->getLocalPlayer() && !mRenderLocal.mValue) return;
    if (gFriendManager->isFriend(actor) && !mShowFriends.mValue) return;
    if (ActorUtils::isBot(actor)) return;
    event.setResult(false); // hides the original nametag
}

void Nametags::onRenderEvent(RenderEvent& event)
{
    auto ci = ClientInstance::get();
    if (!ci->getLevelRenderer()) return;

    auto actors = ActorUtils::getActorList(true, true);
    std::ranges::sort(actors, [&](Actor* a, Actor* b) {
        auto aPosComp = a->getRenderPositionComponent();
        auto bPosComp = b->getRenderPositionComponent();
        if (!aPosComp || !bPosComp) return false;
        auto aPos = aPosComp->mPosition;
        auto bPos = bPosComp->mPosition;
        auto origin = RenderUtils::transform.mOrigin;
        return glm::distance(origin, aPos) > glm::distance(origin, bPos);
    });

    auto drawList = ImGui::GetBackgroundDrawList();

    auto localPlayer = ci->getLocalPlayer();

    for (auto actor : actors)
    {
        if (!actor->isPlayer()) continue;
        if (actor == localPlayer && ci->getOptions()->game_thirdperson->value == 0 && !localPlayer->getFlag<RenderCameraFlag>()) continue;
        if (actor == localPlayer && !mRenderLocal.mValue) continue;
        auto shape = actor->getAABBShapeComponent();
        if (!shape) continue;
        auto posComp = actor->getRenderPositionComponent();
        if (!posComp) continue;

        auto themeColor = ImColor(1.f, 1.f, 1.f, 1.f); //ColorUtils::getThemedColor(0);

        if (gFriendManager->isFriend(actor))
        {
            if (mShowFriends.mValue) themeColor = ImColor(0.0f, 1.0f, 0.0f);
            else continue;
        }

        glm::vec3 renderPos = posComp->mPosition;
        if (actor == localPlayer) renderPos = RenderUtils::transform.mPlayerPos;
        renderPos.y += 0.5f;

        glm::vec3 origin = RenderUtils::transform.mOrigin;
        glm::vec2 screen = glm::vec2(0, 0);

        if (!RenderUtils::transform.mMatrix.OWorldToScreen(origin, renderPos, screen, ci->getFov(), ci->getGuiData()->mResolution)) continue;
        if (std::isnan(screen.x) || std::isnan(screen.y)) continue;
        if (screen.x < 0 || screen.y < 0 || screen.x > ci->getGuiData()->mResolution.x * 2 || screen.y > ci->getGuiData()->mResolution.y * 2) continue;


        float fontSize = mFontSize.mValue;
        float padding = 5.f;

        if (mDistanceScaledFont.mValue)
        {
            // use distance to origin, not actor
            float distance = glm::distance(origin, renderPos) + 2.5f;
            if (distance < 0) distance = 0;
            fontSize = 1.0f / distance * 100.0f * mScalingMultiplier.mValue;
            if (fontSize < 1.0f) fontSize = 1.0f;
            if (fontSize < mMinScale.mValue) fontSize = mMinScale.mValue;
            padding = fontSize / 4;
        }

        FontHelper::pushPrefFont(true, true);

        std::string name = actor->getRawName();

        if (actor == localPlayer)
        {
            name = actor->getNameTag();
            // Remove everything after the first newline
            name = name.substr(0, name.find('\n'));
            name = ColorUtils::removeColorCodes(name);
        }

        ImVec2 imFontSize = ImGui::GetFont()->CalcTextSizeA(fontSize, FLT_MAX, 0, name.c_str());
        ImVec2 pos = ImVec2(screen.x - imFontSize.x / 2, screen.y - imFontSize.y - 5);


        ImVec2 rectMin = ImVec2(pos.x - padding, pos.y - padding);
        ImVec2 rectMax = ImVec2(pos.x + imFontSize.x + padding, pos.y + imFontSize.y + padding);

        drawList->AddShadowRect(rectMin, rectMax, ImColor(0.f, 0.f, 0.f, 1.f), 40.f, ImVec2(0,0));
        drawList->AddRectFilled(rectMin, rectMax, ImColor(0.0f, 0.0f, 0.0f, 0.5f), 10.f);
        drawList->AddText(ImGui::GetFont(), fontSize, pos, themeColor, name.c_str());

        FontHelper::popPrefFont();
    }
}
