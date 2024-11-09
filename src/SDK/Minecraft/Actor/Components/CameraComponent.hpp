//
// Created by vastrakai on 7/22/2024.
//

#pragma once
#include "glm/detail/type_quat.hpp"

// Credit: disabledmallis on discord

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
};
static_assert(sizeof(CameraComponent) == 0x120);

class CameraDirectLookComponent {
public:
    glm::vec2 mRotRads{};
    float mDelta = 0.0f;
    glm::vec2 mWrap{};
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
};

struct CameraAvoidanceComponent
{
    PAD(0x4);
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