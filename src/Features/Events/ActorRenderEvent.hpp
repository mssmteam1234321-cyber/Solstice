//
// Created by vastrakai on 7/8/2024.
//

#pragma once

#include "Event.hpp"
#include <glm/glm.hpp>
#include <memory>
#include <Hook/Hook.hpp>

class ActorRenderDispatcher;
class BaseActorRenderContext;
class Actor;

struct ActorRenderEvent : public CancelableEvent {
    ActorRenderDispatcher* _this;
    BaseActorRenderContext* mEntityRenderContext;
    Actor* mEntity;
    const glm::vec3* mCameraTargetPos;
    const glm::vec3* mPos;
    const glm::vec2* mRot;
    bool mIgnoreLighting;
    Detour* mDetour;

    explicit ActorRenderEvent(ActorRenderDispatcher* _this, BaseActorRenderContext* entityRenderContext, Actor* entity, const glm::vec3* cameraTargetPos, const glm::vec3* pos, const glm::vec2* rot, bool ignoreLighting, Detour* detour) : _this(_this), mEntityRenderContext(entityRenderContext), mEntity(entity), mCameraTargetPos(cameraTargetPos), mPos(pos), mRot(rot), mIgnoreLighting(ignoreLighting), mDetour(detour) {}
};