//
// Created by vastrakai on 6/29/2024.
//

#include "MathUtils.hpp"

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
