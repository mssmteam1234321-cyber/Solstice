//
// Created by vastrakai on 7/22/2024.
//

#pragma once

#include <SDK/Minecraft/Actor/EntityContext.hpp>
#include <SDK/Minecraft/Actor/Components/CameraComponent.hpp>

class LookInputEvent : public CancelableEvent {
public:
    EntityContext* mEntityContext{};
    CameraComponent* mFirstPersonCamera{};
    CameraComponent* mThirdPersonCamera{};
    CameraComponent* mThirdPersonFrontCamera{};
    CameraDirectLookComponent* mCameraDirectLookComponent{};
    glm::vec2 mVec2{};

    explicit LookInputEvent(EntityContext* entityContext, CameraComponent* firstPersonCamera, CameraComponent* thirdPersonCamera, CameraComponent* thirdPersonFrontCamera, CameraDirectLookComponent* cameraDirectLookComponent, glm::vec2 vec2) : mEntityContext(entityContext), mFirstPersonCamera(firstPersonCamera), mThirdPersonCamera(thirdPersonCamera), mThirdPersonFrontCamera(thirdPersonFrontCamera), mCameraDirectLookComponent(cameraDirectLookComponent), mVec2(vec2) {}
};