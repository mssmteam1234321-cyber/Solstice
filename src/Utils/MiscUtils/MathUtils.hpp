#pragma once
//
// Created by vastrakai on 6/29/2024.
//

#include <cmath>

#include <glm/glm.hpp>

class MathUtils {
public:
    static float animate(float from, float to, float speed);
    static float lerp(float a, float b, float t);
    static float getRotationKeyOffset();
    static glm::vec2 getMotion(float yaw, float speed);
};
