//
// Created by vastrakai on 6/29/2024.
//

#include "MathUtils.hpp"

#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/Actor/Components/MoveInputComponent.hpp>

#include <glm/glm.hpp>
#include <Utils/Keyboard.hpp>


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
