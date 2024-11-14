//
// Created by vastrakai on 7/22/2024.
//

#pragma once
#include "glm/detail/type_quat.hpp"

// Credit: disabledmallis on discord

enum class CameraMode : int {
    FirstPerson = 0,
    ThirdPerson = 1,
    ThirdPersonFront = 2,
    DeathCamera = 3,
    Free = 4,
    FollowOrbit = 5,
};

class CameraComponent {
public:
    HashedString mViewName{};
    glm::quat mQuat{};
    glm::vec3 mOrigin{};
    glm::vec4 mFov{}; // mViewport? (Fov X, Fov Y, Near, Far)?
    glm::mat4 mWorld{};
    glm::mat4 mView{};
    glm::mat4 mProjection{};
    int8_t padding[4]{0};

    void copyFrom(const CameraComponent* other) {
        mQuat = other->mQuat;
        mOrigin = other->mOrigin;
        mFov = other->mFov;
        mWorld = other->mWorld;
        mView = other->mView;
        mProjection = other->mProjection;
    }

    [[nodiscard]] CameraMode getMode() const {
        if (mViewName.text == "minecraft:first_person") return CameraMode::FirstPerson;
        if (mViewName.text == "minecraft:third_person") return CameraMode::ThirdPerson;
        if (mViewName.text == "minecraft:third_person_front") return CameraMode::ThirdPersonFront;
        if (mViewName.text == "minecraft:death_camera") return CameraMode::DeathCamera;
        if (mViewName.text == "minecraft:free") return CameraMode::Free;
        if (mViewName.text == "minecraft:follow_orbit") return CameraMode::FollowOrbit;

        return CameraMode::FirstPerson;
    }
};
static_assert(sizeof(CameraComponent) == 0x120);

class CameraDirectLookComponent {
public:
    glm::vec2 mRotRads{};
    float mDelta = 0.0f;
    glm::vec2 mWrap{};

    void operator=(const CameraDirectLookComponent& other) {
        mRotRads = other.mRotRads;
        mDelta = other.mDelta;
        mWrap = other.mWrap;
    }
};


class CameraOrbitComponent
{
public:
    PAD(0x44); // Padding to match size

    CLASS_FIELD(float, mRadius, 0x28);
    CLASS_FIELD(glm::vec2, mRotRads, 0x2C);
};

class DebugCameraComponent
{
public:
    HashedString mViewName{};
    glm::quat mQuat{};
    glm::vec3 mOrigin{};
    glm::vec4 mFov{}; // mViewport? (Fov X, Fov Y, Near, Far)?
    glm::mat4 mWorld{};
    glm::mat4 mView{};
    glm::mat4 mProjection{};
    int8_t padding[4]{0};
};

class UpdatePlayerFromCameraComponent
{
public:
    int mUpdateMode = 0;

    UpdatePlayerFromCameraComponent() = default;
    UpdatePlayerFromCameraComponent(int mode) : mUpdateMode(mode) {}
};

struct CameraAvoidanceComponent
{
    PAD(0x8);
};

struct CameraBobComponent
{
    PAD(0x4);
};

struct ShadowOffsetComponent
{
    float mOffset = 0.0f;
};

struct SubBBsComponent
{
    std::vector<AABB> mSubBBs;
};