//
// Created by vastrakai on 6/29/2024.
//

#include "MathUtils.hpp"

#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/Actor/Components/MoveInputComponent.hpp>

#include <glm/glm.hpp>
#include <Utils/Keyboard.hpp>


float MathUtils::animate(float from, float to, float speed)
{
    speed = (speed < 0.0) ? 0.0 : ((speed > 1.0) ? 1.0 : speed);
    float diff = fmax(to, from) - fmin(to, from);
    float factor = diff * speed;
    return from + (to > from ? factor : -factor);
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
