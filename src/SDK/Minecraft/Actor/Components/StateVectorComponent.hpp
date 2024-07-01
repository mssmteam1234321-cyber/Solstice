//
// Created by vastrakai on 6/30/2024.
//

#pragma once

#include <glm/vec3.hpp>

struct StateVectorComponent {
    glm::vec3 mPos;
    glm::vec3 mPosOld;
    glm::vec3 mVelocity;
};
