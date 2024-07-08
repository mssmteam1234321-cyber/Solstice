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

float MathUtils::random(float min, float max)
{
    return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
}

int MathUtils::random(int min, int max)
{
    return min + (rand() % (max - min + 1));
}

std::vector<glm::vec2> MathUtils::getBoxPoints(const AABB& aabb)
{
    ClientInstance* ci = ClientInstance::get();
    if (!ci->getLocalPlayer()) return {};

    auto corrected = RenderUtils::transform.mMatrix;

    glm::vec3 worldPoints[8];

    worldPoints[0] = glm::vec3(aabb.mMin.x, aabb.mMin.y, aabb.mMin.z);
    worldPoints[1] = glm::vec3(aabb.mMin.x, aabb.mMin.y, aabb.mMax.z);
    worldPoints[2] = glm::vec3(aabb.mMax.x, aabb.mMin.y, aabb.mMin.z);
    worldPoints[3] = glm::vec3(aabb.mMax.x, aabb.mMin.y, aabb.mMax.z);
    worldPoints[4] = glm::vec3(aabb.mMin.x, aabb.mMax.y, aabb.mMin.z);
    worldPoints[5] = glm::vec3(aabb.mMin.x, aabb.mMax.y, aabb.mMax.z);
    worldPoints[6] = glm::vec3(aabb.mMax.x, aabb.mMax.y, aabb.mMin.z);
    worldPoints[7] = glm::vec3(aabb.mMax.x, aabb.mMax.y, aabb.mMax.z);

    std::vector<glm::vec2> points;
    for (int i = 0; i < 8; i++)
    {
        glm::vec2 result;
        if (!corrected.OWorldToScreen(origin, worldPoints[i], result, fov, displaySize)) return {};
        if (result.x < 0 || result.y < 0 || result.x > displaySize.x || result.y > displaySize.y) return {};
        points.push_back(result);
    };

    if (points.size() < 3)
        return {};

    // Find start vertex
    auto startIt = std::min_element(points.begin(), points.end(), [](const auto& lhs, const auto& rhs) {
        return lhs.x < rhs.x;
    });
    glm::vec2 start = *startIt;

    // Follow outline
    std::vector<glm::vec2> outline;
    glm::vec2 current = start;
    outline.push_back(current);
    float lastDirAtan2 = atan2(0, -1);

    do {
        float smallestAngle = IM_PI * 2;
        glm::vec2 smallestDir = { 0, 0 };
        glm::vec2 smallestE = { 0, 0 };
        for (const auto& e : points)
        {
            if (e == current) continue;
            glm::vec2 dir = e - current;
            float angle = atan2(dir.y, dir.x) - lastDirAtan2;
            if (angle > IM_PI)
                angle -= IM_PI * 2;
            else if (angle <= -IM_PI)
                angle += IM_PI * 2;
            if (angle >= 0 && angle < smallestAngle) {
                smallestAngle = angle;
                smallestDir = dir;
                smallestE = e;
            }
        }

        if (smallestE.x != 0 && smallestE.y != 0)
            outline.push_back(smallestE);
        lastDirAtan2 = atan2(smallestDir.y, smallestDir.x);
        current = smallestE;
    } while (current != start && outline.size() < 8);

    return outline;
}
