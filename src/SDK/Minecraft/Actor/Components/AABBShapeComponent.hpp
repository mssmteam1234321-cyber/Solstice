//
// Created by vastrakai on 6/28/2024.
//

#pragma once

#include <glm/vec3.hpp>

struct AABBShapeComponent {
    glm::vec3 mMin;
    glm::vec3 mMax;
    float mWidth;
    float mHeight;
};