//
// Created by vastrakai on 6/29/2024.
//

#include "MathUtils.hpp"

#include <imgui_internal.h>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/Actor/Components/MoveInputComponent.hpp>

#include <glm/glm.hpp>
#include <Utils/Keyboard.hpp>

#include "RenderUtils.hpp"
#include "spdlog/spdlog.h"


float MathUtils::animate(float endPoint, float current, float speed) { // Animate the position
    if (speed < 0.0) speed = 0.0; // If 0 is less than speed then set speed to 0.
    else if (speed > 1.0) speed = 1.0; // If Speed is faster than 1 then set speed to 1.

    float dif = std::fmax(endPoint, current) - std::fmin(endPoint, current); // Animate between max and min.
    float factor = dif * speed;
    return current + (endPoint > current ? factor : -factor); // Animates the distance
}

float MathUtils::lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

float MathUtils::getRotationKeyOffset()
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return 0;
    auto moveInput = player->getMoveInputComponent();

    bool isMoving = moveInput->mForward || moveInput->mBackward || moveInput->mLeft || moveInput->mRight;
    if (!isMoving) return 0;

    bool w = moveInput->mForward;
    bool s = moveInput->mBackward;
    bool a = moveInput->mLeft;
    bool d = moveInput->mRight;

    float yawOffset = 0;
    if (w && a)
        yawOffset = -45;
    else if (w && d)
        yawOffset = 45;
    else if (s && a)
        yawOffset = -135;
    else if (s && d)
        yawOffset = 135;
    else if (w)
        yawOffset = 0;
    else if (a)
        yawOffset = -90;
    else if (s)
        yawOffset = -180;
    else if (d)
        yawOffset = 90;
    else
        yawOffset = 0;

    return yawOffset;
}


glm::vec2 MathUtils::getMotion(float yaw, float speed) {
    yaw += getRotationKeyOffset() + 90;

    float calcYaw = glm::radians(yaw);

    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return { 0, 0 };

    if (!Keyboard::isUsingMoveKeys()) return { 0, 0 };

    glm::vec2 motion;
    motion.x = cos(calcYaw) * (speed);
    motion.y = sin(calcYaw) * (speed);

    return motion;
}

float MathUtils::wrap(float val, float min, float max)
{
    return fmod(fmod(val - min, max - min) + (max - min), max - min) + min;
}

float MathUtils::random(float min, float max)
{
    return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
}

int MathUtils::random(int min, int max)
{
    return min + (rand() % (max - min + 1));
}

#define PI IM_PI

std::vector<glm::vec2> MathUtils::getBoxPoints(const AABB& aabb) {
    ClientInstance* ci = ClientInstance::get();
    if (!ci->getLocalPlayer()) return {};

    auto& corrected = RenderUtils::transform.mMatrix;

    glm::vec3 worldPoints[8] = {
        {aabb.mMin.x, aabb.mMin.y, aabb.mMin.z},
        {aabb.mMin.x, aabb.mMin.y, aabb.mMax.z},
        {aabb.mMax.x, aabb.mMin.y, aabb.mMin.z},
        {aabb.mMax.x, aabb.mMin.y, aabb.mMax.z},
        {aabb.mMin.x, aabb.mMax.y, aabb.mMin.z},
        {aabb.mMin.x, aabb.mMax.y, aabb.mMax.z},
        {aabb.mMax.x, aabb.mMax.y, aabb.mMin.z},
        {aabb.mMax.x, aabb.mMax.y, aabb.mMax.z}
    };

    std::vector<glm::vec2> points;
    points.reserve(8);
    for (const auto& wp : worldPoints) {
        glm::vec2 result = {0, 0};
        if (!corrected.OWorldToScreen(origin, wp, result, fov, displaySize)) return {};
        if (result != glm::vec2(FLT_MAX, FLT_MAX)) {
            points.push_back(result);
        } else {
            return {};
        }
    }

    if (points.size() < 3) return {};

    auto it = std::ranges::min_element(points, [](const glm::vec2& a, const glm::vec2& b) {
        return a.x < b.x;
    });
    glm::vec2 start = *it;

    std::vector<glm::vec2> indices;
    indices.reserve(8);
    indices.push_back(start);

    glm::vec2 current = start;
    glm::vec2 lastDir = glm::vec2(0, -1);

    do {
        float smallestAngle = 2 * PI;
        glm::vec2 smallestDir = {0, 0};
        glm::vec2 smallestE = {0, 0};
        float lastDirAtan2 = atan2(lastDir.y, lastDir.x);

        for (const auto& t : points) {
            if (current == t) continue;

            glm::vec2 dir = t - current;
            float angle = atan2(dir.y, dir.x) - lastDirAtan2;
            if (angle > PI) angle -= 2 * PI;
            else if (angle <= -PI) angle += 2 * PI;

            if (angle >= 0 && angle < smallestAngle) {
                smallestAngle = angle;
                smallestDir = dir;
                smallestE = t;
            }
        }

        indices.push_back(smallestE);
        lastDir = smallestDir;
        current = smallestE;

    } while (current != start && indices.size() < 8);

    return indices;
}

glm::vec2 MathUtils::getRots(const glm::vec3& pEyePos, const glm::vec3& pTarget)
{
    glm::vec3 delta = pTarget - pEyePos;
    const float yaw = atan2(delta.z, delta.x) * 180.0f / IM_PI;
    const float pitch = atan2(delta.y, sqrt(delta.x * delta.x + delta.z * delta.z)) * 180.0f / IM_PI;
    return {-pitch, yaw - 90};
}

glm::vec2 MathUtils::getRots(const glm::vec3& pEyePos, const AABB& target)
{
    return getRots(pEyePos, target.getClosestPoint(pEyePos));
}
