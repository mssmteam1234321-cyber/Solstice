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
    hurtTimeAnimPerc = MathUtils::lerp(hurtTimeAnimPerc, lerpedHurtTime, delta * 20.f);
    static float healthAnimPerc = 0.f;
    static float absorptionAnimPerc = 0.f;

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

    FontHelper::pushPrefFont(true, false);

    auto screenSize = ImGui::GetIO().DisplaySize;

    auto boxSize = ImVec2(300 * anim, 115 * anim);
    auto boxPos = ImVec2(screenSize.x / 2 - boxSize.x / 2 + mXOffset.mValue, screenSize.y / 2 - boxSize.y / 2 + mYOffset.mValue);

    ImVec2 headSize = ImVec2(65 * anim, 65 * anim);
    ImVec2 headPos = ImVec2(boxPos.x + 10 * anim, boxPos.y + 10 * anim);
    ImVec2 headSize2 = ImVec2(MathUtils::lerp(headSize.x, 40 * anim, hurtTimeAnimPerc), MathUtils::lerp(headSize.y, 40 * anim, hurtTimeAnimPerc));

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

    ImColor imageColor = ImColor(1.f, 1.f, 1.f, 1.f * anim);

    // Adjust the color based on hurttime (red tint)
    imageColor.Value.x = MathUtils::lerp(imageColor.Value.x, 1.f, hurtTimeAnimPerc);
    imageColor.Value.y = MathUtils::lerp(imageColor.Value.y, 1.f - hurtTimeAnimPerc, hurtTimeAnimPerc);
    imageColor.Value.z = MathUtils::lerp(imageColor.Value.z, 1.f - hurtTimeAnimPerc, hurtTimeAnimPerc);

    headPos.x += (headSize.x - headSize2.x) / 2;
    headPos.y += (headSize.y - headSize2.y) / 2;

    drawList->AddImage(texture, headPos, headPos + headSize2, ImVec2(0, 0), ImVec2(1, 1), imageColor);

    ImVec2 healthBarStart = ImVec2(boxPos.x + 85 * anim, boxPos.y + 59 * anim);
    ImVec2 healthBarEnd = ImVec2(boxPos.x + boxSize.x - 30 * anim, boxPos.y + 78 * anim);

    drawList->AddRectFilled(healthBarStart, healthBarEnd,
                                    ImColor(149, 130, 133, (int)((float)194 * anim)), 10.f);

    if (healthAnimPerc > 0.01f)
        drawList->AddRectFilled(healthBarStart, ImVec2(healthBarStart.x + (healthBarEnd.x - healthBarStart.x) * healthAnimPerc, healthBarEnd.y),
                                 ImColor(213, 187, 190, (int) (255 * anim)), 10.f);



    if (absorptionAnimPerc > 0.01f)
        drawList->AddRectFilled(healthBarStart,
                                    ImVec2(healthBarStart.x + (healthBarEnd.x - healthBarStart.x) * absorptionAnimPerc, healthBarEnd.y),
                                 ImColor(234, 168, 95, (int)(255 * anim)), 10.f);


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
