//
// Created by vastrakai on 7/22/2024.
//

#include "LookInputHook.hpp"

#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/MinecraftSim.hpp>
#include <Features/Events/LookInputEvent.hpp>

std::unique_ptr<Detour> LookInputHook::mDetour;

void LookInputHook::_handleLookInput(EntityContext* entityContext, CameraComponent& cameraComponent,
    CameraDirectLookComponent& cameraDirectLookComponent, glm::vec2 const& vec2)
{
    auto original = mDetour->getOriginal<decltype(_handleLookInput)*>();

    auto thirdPersonCamera = reinterpret_cast<CameraComponent*>(reinterpret_cast<uintptr_t>(&cameraComponent) + 0x120);
    auto thirdPersonFront = reinterpret_cast<CameraComponent*>(reinterpret_cast<uintptr_t>(&cameraComponent) + 0x120 * 2);

    auto holder = nes::make_holder<LookInputEvent>(entityContext, &cameraComponent, thirdPersonCamera, thirdPersonFront, &cameraDirectLookComponent, vec2);
    gFeatureManager->mDispatcher->trigger(holder);
    if (holder->isCancelled()) return;

    return original(entityContext, cameraComponent, cameraDirectLookComponent, vec2);
}

void LookInputHook::init()
{
    mDetour = std::make_unique<Detour>("CameraDirectLookSystemUtil::_handleLookInput", reinterpret_cast<void*>(SigManager::CameraDirectLookSystemUtil_handleLookInput), &_handleLookInput);
}
