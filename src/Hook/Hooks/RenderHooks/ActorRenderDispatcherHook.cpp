//
// Created by vastrakai on 7/8/2024.
//

#include "ActorRenderDispatcherHook.hpp"
#include <Features/Events/ActorRenderEvent.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>

std::unique_ptr<Detour> ActorRenderDispatcherHook::mDetour;

void ActorRenderDispatcherHook::render(ActorRenderDispatcher* _this, BaseActorRenderContext* entityRenderContext,
    Actor* entity, const glm::vec3* cameraTargetPos, const glm::vec3* pos, const glm::vec2* rot, bool ignoreLighting)
{
    auto oFunc = mDetour->getOriginal<decltype(&render)>();
    auto localPlayer = ClientInstance::get()->getLocalPlayer();
    if (!localPlayer) return;
    if (entity != localPlayer)
    {
        return oFunc(_this, entityRenderContext, entity, cameraTargetPos, pos, rot, ignoreLighting);
    }

    auto holder = nes::make_holder<ActorRenderEvent>(_this, entityRenderContext, entity, cameraTargetPos, pos, rot, ignoreLighting, mDetour.get());
    gFeatureManager->mDispatcher->trigger(holder);
    if (holder->isCancelled()) return;

    return oFunc(_this, entityRenderContext, entity, cameraTargetPos, pos, rot, ignoreLighting);
}

void ActorRenderDispatcherHook::init()
{
    static bool once = false;
    if (once) return;
    once = true;

    mName = "ActorRenderDispatcher::render";
    mDetour = std::make_unique<Detour>("ActorRenderDispatcher::render", reinterpret_cast<void*>(SigManager::ActorRenderDispatcher_render), &ActorRenderDispatcherHook::render);
    mDetour->enable();
}
