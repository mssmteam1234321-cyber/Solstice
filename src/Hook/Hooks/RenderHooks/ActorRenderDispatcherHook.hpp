#pragma once
//
// Created by vastrakai on 7/8/2024.
//

#include <Hook/Hook.hpp>
#include <Hook/HookManager.hpp>
#include <glm/glm.hpp>

class ActorRenderDispatcher;
class BaseActorRenderContext;
class Actor;

class ActorRenderDispatcherHook : public Hook {
public:
    ActorRenderDispatcherHook() : Hook() {

    }

    static std::unique_ptr<Detour> mDetour;

    static void render(ActorRenderDispatcher* _this, class BaseActorRenderContext* entityRenderContext, Actor* entity, const glm::vec3* cameraTargetPos, const glm::vec3* pos, const glm::vec2* rot, bool ignoreLighting);
    void init() override;
};

