#pragma once
//
// Created by vastrakai on 6/29/2024.
//

#include <atomic>
#include <cmath>

#include <glm/glm.hpp>

class MathUtils {
public:
    static float animate(float endPoint, float current, float speed);
    static float lerp(float a, float b, float t);
    static float getRotationKeyOffset();
    static glm::vec2 getMotion(float yaw, float speed);
    template <typename T>
    static T clamp(T value, T min, T max) {
        return std::max(min, std::min(value, max));
    }
    static float clamp(float value, float min, float max) {
        return std::max(min, std::min(value, max));
    }

    // random
    static float random(float min, float max);
    static int random(int min, int max);
    template <typename T>
    static T random(T min, T max) {
        return random(static_cast<T>(min), static_cast<T>(max));
    }
};
