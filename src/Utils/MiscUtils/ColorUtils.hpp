#pragma once
//
// Created by vastrakai on 7/1/2024.
//

#include <chrono>
#include <imgui.h>

#define NOW std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count()

class ColorUtils {
public:
    static ImColor Rainbow(float seconds, float saturation, float brightness, int index);
};