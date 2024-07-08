//
// Created by vastrakai on 7/7/2024.
//

#include "SetupAndRenderHook.hpp"

#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/Rendering/LevelRenderer.hpp>

#include "D3DHook.hpp"

std::unique_ptr<Detour> SetupAndRenderHook::mDetour;

void SetupAndRenderHook::onSetupAndRender(void* screenView, void* mcuirc)
{
    auto original = mDetour->getOriginal<decltype(&onSetupAndRender)>();

    auto ci = ClientInstance::get();
    if (!ci) return original(screenView, mcuirc);

    auto player = ClientInstance::get()->getLocalPlayer();

    glm::vec3 origin = glm::vec3(0, 0, 0);
    glm::vec3 playerPos = glm::vec3(0, 0, 0);

    if (player && ci->getLevelRenderer())
    {
        origin = *ci->getLevelRenderer()->getRendererPlayer()->getCameraPos();
        playerPos = player->getRenderPositionComponent()->mPosition;
    }

    D3DHook::FrameTransforms.push({ ci->getViewMatrix(), origin, playerPos });

    original(screenView, mcuirc);
}

void SetupAndRenderHook::init()
{
    static bool initialized = false;
    if (initialized) return;
    initialized = true;

    mName = "ScreenView::setupAndRender";
    mDetour = std::make_unique<Detour>("ScreenView::setupAndRender", reinterpret_cast<void*>(SigManager::ScreenView_setupAndRender), &SetupAndRenderHook::onSetupAndRender);
    mDetour->enable();
}
