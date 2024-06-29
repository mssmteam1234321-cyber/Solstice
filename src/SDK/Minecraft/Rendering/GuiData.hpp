#pragma once
#include <string>
#include <Utils/MemUtils.hpp>

#include "glm/vec2.hpp"
//
// Created by vastrakai on 6/25/2024.
//


class GuiData {
public:
    // unlikely to change
    CLASS_FIELD(glm::vec2, resolution, 0x30);
    CLASS_FIELD(glm::vec2, resolutionRounded, 0x38);
    CLASS_FIELD(glm::vec2, resolutionScaled, 0x40);

    CLASS_FIELD(float, guiScale, 0x4C);
    CLASS_FIELD(float, scalingMultiplier, 0x50);

    void displayClientMessage(const std::string& msg);
};