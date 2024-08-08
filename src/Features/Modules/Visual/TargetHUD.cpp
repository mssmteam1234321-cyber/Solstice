//
// Created by vastrakai on 8/4/2024.
//

#include "TargetHUD.hpp"

#include <Features/Events/BaseTickEvent.hpp>
#include <Features/Events/PacketInEvent.hpp>
#include <Features/Modules/Combat/Aura.hpp>
#include <Hook/Hooks/RenderHooks/D3DHook.hpp>
#include <SDK/Minecraft/Network/Packets/ActorEventPacket.hpp>

void TargetHUD::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &TargetHUD::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketInEvent, &TargetHUD::onPacketInEvent>(this);
}

void TargetHUD::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &TargetHUD::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketInEvent, &TargetHUD::onPacketInEvent>(this);
}

void TargetHUD::onBaseTickEvent(BaseTickEvent& event)
{
    if (Aura::sHasTarget && Aura::sTarget && Aura::sTarget->getMobHurtTimeComponent())
    {
        if (!Aura::sTarget->getActorTypeComponent())
        {
            spdlog::warn("TargetHUD: Target has no ActorTypeComponent");
            Aura::sTarget = nullptr;
            return;
        }
        mLastHurtTime = mHurtTime;
        mHurtTime = static_cast<float>(Aura::sTarget->getMobHurtTimeComponent()->mHurtTime);
        mLastHealth = mHealth;
        mLastAbsorption = mAbsorption;
        mLastMaxHealth = mMaxHealth;
        mLastMaxAbsorption = mMaxAbsorption;
        mHealth = Aura::sTarget->getHealth();
        mMaxHealth = Aura::sTarget->getMaxHealth();
        mAbsorption = Aura::sTarget->getAbsorption();
        mMaxAbsorption = Aura::sTarget->getMaxAbsorption();
        mLastPlayerName = Aura::sTarget->getRawName();

        if (mHurtTime > mLastHurtTime)
        {
            mLastHurtTime = mHurtTime;
        }
    }
}

void TargetHUD::onRenderEvent(RenderEvent& event)
{
    auto drawList = ImGui::GetBackgroundDrawList();
    if (mStyle.mValue != Style::Solstice)
    {
        return;
    }

    static float anim = 0.f;

    float delta = ImGui::GetIO().DeltaTime;


    float lerpedHurtTime = MathUtils::lerp(mLastHurtTime / 10.f, mHurtTime / 10.f, delta);

    static float hurtTimeAnimPerc = 0.f;
    static float healthAnimPerc = 0.f;
    static float absorptionAnimPerc = 0.f;

    // if the last target is different, recalc immediately
    if (mLastTarget != Aura::sTarget)
    {
        mLastTarget = Aura::sTarget;
        mLastHealth = mHealth;
        mLastAbsorption = mAbsorption;
        mLastMaxHealth = mMaxHealth;
        mLastMaxAbsorption = mMaxAbsorption;
        healthAnimPerc = mHealth / mMaxHealth;
        absorptionAnimPerc = mAbsorption / 20.f;
        mLerpedHealth = mHealth;
        mLerpedAbsorption = mAbsorption;
        spdlog::info("Recalcing health and absorption");
    }

    mLastTarget = Aura::sTarget;

    hurtTimeAnimPerc = MathUtils::lerp(hurtTimeAnimPerc, lerpedHurtTime, delta * 20.f);

    float perc = mLastHealth / mLastMaxHealth;
    healthAnimPerc = MathUtils::lerp(healthAnimPerc, perc, delta * 6.f);
    float perc2 = mLastAbsorption / 20.f;
    absorptionAnimPerc = MathUtils::lerp(absorptionAnimPerc, perc2, delta * 6.f);

    // lerp health and absorption
    mLerpedHealth = MathUtils::lerp(mLerpedHealth, mHealth, delta * 10.f);
    mLerpedAbsorption = MathUtils::lerp(mLerpedAbsorption, mAbsorption, delta * 10.f);

    bool showing = mEnabled && Aura::sHasTarget && Aura::sTarget;

    anim = MathUtils::lerp(anim, showing ? 1.f : 0.f, ImGui::GetIO().DeltaTime * 10.f);

    if (anim < 0.01f)
    {
        return;
    }

    FontHelper::pushPrefFont(true, true);

    auto screenSize = ImGui::GetIO().DisplaySize;

    auto boxSize = ImVec2(255 * anim, 115 * anim);
    auto boxPos = ImVec2(screenSize.x / 2 - boxSize.x / 2 + mXOffset.mValue, screenSize.y / 2 - boxSize.y / 2 + mYOffset.mValue);

    auto headSize = ImVec2(65 * anim, 65 * anim);
    auto headPos = ImVec2(boxPos.x + 10 * anim, boxPos.y + 10 * anim);
    auto headSize2 = ImVec2(MathUtils::lerp(headSize.x, 40 * anim, hurtTimeAnimPerc), MathUtils::lerp(headSize.y, 40 * anim, hurtTimeAnimPerc));

    drawList->AddRectFilled(boxPos, ImVec2(boxPos.x + boxSize.x, boxPos.y + boxSize.y), ImColor(0.f, 0.f, 0.f, 0.5f * anim), 15.f * anim);

    static std::string filePath = "notch.png";
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

    auto imageColor = ImColor(1.f, 1.f, 1.f, 1.f * anim);

    // Adjust the color based on hurttime (red tint)
    imageColor.Value.x = MathUtils::lerp(imageColor.Value.x, 1.f, hurtTimeAnimPerc);
    imageColor.Value.y = MathUtils::lerp(imageColor.Value.y, 1.f - hurtTimeAnimPerc, hurtTimeAnimPerc);
    imageColor.Value.z = MathUtils::lerp(imageColor.Value.z, 1.f - hurtTimeAnimPerc, hurtTimeAnimPerc);

    float startY = headPos.y + (headSize.y + 10 * anim);

    std::string name = mLastPlayerName;
    auto textNameSize = ImGui::GetFont()->CalcTextSizeA(20 * anim, FLT_MAX, 0, name.c_str());
    auto textNamePos = ImVec2(headPos.x + headSize.x + 10 * anim, headPos.y + 10 * anim);

    std::string healthStr = "Health: " + std::to_string(static_cast<int>(mHealth + mAbsorption));
    auto textHealthSize = ImGui::GetFont()->CalcTextSizeA(20 * anim, FLT_MAX, 0, healthStr.c_str());
    auto textHealthPos = ImVec2(headPos.x + headSize.x + 10 * anim, headPos.y + (10 * anim) + textNameSize.y + (10 * anim));

    headPos.x += (headSize.x - headSize2.x) / 2;
    headPos.y += (headSize.y - headSize2.y) / 2;

    drawList->AddImageRounded(texture, headPos, headPos + headSize2, ImVec2(0, 0), ImVec2(1, 1), imageColor, 10.f * anim);

    //drawList->AddText(ImGui::GetFont(), 20 * anim, textPos, ImColor(255, 255, 255, 255), name.c_str());
    auto textStartPos = textNamePos;
    auto textEndPos = textHealthPos + textHealthSize;
    textEndPos.x = boxPos.x + boxSize.x - 10 * anim;
    drawList->PushClipRect(textStartPos, textEndPos, true); // So that the text doesn't go outside the box
    ImRenderUtils::drawShadowText(drawList, name, textNamePos, ImColor(255, 255, 255, static_cast<int>(255 * anim)), 20 * anim, false);
    ImRenderUtils::drawShadowText(drawList, healthStr, textHealthPos, ImColor(255, 255, 255, static_cast<int>(255 * anim)), 20 * anim, false);
    drawList->PopClipRect();


    float ysize = 19 * anim;
    auto healthBarStart = ImVec2(boxPos.x + 10 * anim, startY);
    int barSizeX = boxSize.x - 10;
    auto healthBarEnd = ImVec2(boxPos.x + barSizeX, startY + ysize);


    // Health bar background
    drawList->AddRectFilled(healthBarStart, healthBarEnd, ImColor(149, 130, 133, (int)((float)194 * anim)), 10.f);

    // Health bar
    float healthPerc = mLerpedHealth / mMaxHealth;
    healthPerc = MathUtils::clamp(healthPerc, 0.f, 1.f);
    if (healthPerc > 0.01f)
    {
        auto barEnd = ImVec2(healthBarStart.x + (barSizeX * healthPerc), healthBarEnd.y);
        barEnd.x = MathUtils::clamp(barEnd.x, healthBarStart.x, healthBarEnd.x);
        drawList->AddRectFilled(healthBarStart, barEnd,
                                 ImColor(211, 0, 201, (int) (255 * anim)), 10.f);
    }

    // Absorption bar
    float absorptionPerc = mLerpedAbsorption / 20.f;
    absorptionPerc = MathUtils::clamp(absorptionPerc, 0.f, 1.f);
    if (absorptionPerc > 0.01f)
    {
        auto barEnd = ImVec2(healthBarStart.x + (barSizeX * absorptionPerc), healthBarEnd.y);
        barEnd.x = MathUtils::clamp(barEnd.x, healthBarStart.x, healthBarEnd.x);
        drawList->AddRectFilled(healthBarStart,
                                    barEnd,
                                 ImColor(234, 168, 95, (int)(255 * anim)), 10.f);
    }

    FontHelper::popFont();
}

void TargetHUD::onPacketInEvent(PacketInEvent& event)
{
    if (event.mPacket->getId() == PacketID::ActorEvent)
    {
        auto packet = event.getPacket<ActorEventPacket>();

        if (packet->mEvent != ActorEvent::HURT) return;

        Actor* target = ActorUtils::getActorFromRuntimeID(packet->mRuntimeID);
        if (!target) return;
    }
}
