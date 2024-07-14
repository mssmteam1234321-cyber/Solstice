//
// Created by vastrakai on 7/2/2024.
//

#include "LevelInfo.hpp"

#include <Features/FeatureManager.hpp>
#include <Features/Events/BaseTickEvent.hpp>
#include <Features/Events/PingUpdateEvent.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/MinecraftSim.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>

void LevelInfo::onEnable()
{
    gFeatureManager->mDispatcher->listen<RenderEvent, &LevelInfo::onRenderEvent>(this);
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &LevelInfo::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->listen<PingUpdateEvent, &LevelInfo::onPingUpdateEvent>(this);
}

void LevelInfo::onDisable()
{
    gFeatureManager->mDispatcher->deafen<RenderEvent, &LevelInfo::onRenderEvent>(this);
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &LevelInfo::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->deafen<PingUpdateEvent, &LevelInfo::onPingUpdateEvent>(this);
}

void LevelInfo::onPingUpdateEvent(PingUpdateEvent& event)
{
    mPing = event.mPing;
}


void LevelInfo::onBaseTickEvent(BaseTickEvent& event)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    static glm::vec3 posPrev = *player->getPos();
    glm::vec3 pos = *player->getPos();
    // Ignore y in the BPS calculation

    glm::vec2 posxz = { pos.x, pos.z };
    glm::vec2 posPrevxz = { posPrev.x, posPrev.z };


    float bps = glm::distance(posxz, posPrevxz) * (ClientInstance::get()->getMinecraftSim()->getSimTimer() * ClientInstance::get()->getMinecraftSim()->getSimSpeed());
    mBps = bps;
    mBpsHistory[NOW] = bps;

    for (auto it = mBpsHistory.begin(); it != mBpsHistory.end();)
    {
        if (NOW - it->first > 1000) it = mBpsHistory.erase(it);
        else ++it;
    }

    // Average the BPS
    float total = 0.f;
    int count = 0;
    for (auto it = mBpsHistory.begin(); it != mBpsHistory.end(); ++it)
    {
        total += it->second;
        count++;
    }
    mAveragedBps = total / count;
    posPrev = pos;
}

void LevelInfo::onRenderEvent(RenderEvent& event)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (ClientInstance::get()->getMouseGrabbed() && player) return;

    ImGui::PushFont(FontHelper::Fonts["mojangles_large"]);

    ImVec2 pos = { 2, ImGui::GetIO().DisplaySize.y };
    float fontSize = 20.f;
    std::string fpsText = "FPS: " + std::to_string((int)ImGui::GetIO().Framerate);
    float fontY = ImGui::GetFont()->CalcTextSizeA(fontSize, FLT_MAX, -1.f, fpsText.c_str()).y;
    pos.y -= fontY + 2.f;



    if (mShowBPS.mValue && player)
    {
        std::string bpsText = "BPS: " + fmt::format("{:.2f}", mBps) + " (" + fmt::format("{:.2f}", mAveragedBps) + ")";
        ImRenderUtils::drawShadowText(ImGui::GetBackgroundDrawList(), bpsText, pos, ImColor(1.f, 1.f, 1.f, 1.f), fontSize);
        pos.y -= fontY + 2.f;
    }

    if (mShowXYZ.mValue && player)
    {
        glm::ivec3 pPos = *player->getPos();
        std::string xyzText = "XYZ: " + std::to_string(pPos.x) + "/" + std::to_string(pPos.y) + "/" + std::to_string(pPos.z);
        ImRenderUtils::drawShadowText(ImGui::GetBackgroundDrawList(), xyzText, pos, ImColor(1.f, 1.f, 1.f, 1.f), fontSize);
        pos.y -= fontY + 2.f;
    }

    if (mShowPing.mValue)
    {
        std::string pingText = "Ping: " + std::to_string(mPing) + "ms";
        ImRenderUtils::drawShadowText(ImGui::GetBackgroundDrawList(), pingText, pos, ImColor(1.f, 1.f, 1.f, 1.f), fontSize);
        pos.y -= fontY + 2.f;
    }

    if (mShowFPS.mValue && ClientInstance::get()->getScreenName() != "start_screen")
    {
        ImRenderUtils::drawShadowText(ImGui::GetBackgroundDrawList(), fpsText, pos, ImColor(1.f, 1.f, 1.f, 1.f), fontSize);
        pos.y -= fontY + 2.f;
    }


    ImGui::PopFont();

}
