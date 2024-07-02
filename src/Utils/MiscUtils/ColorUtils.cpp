//
// Created by vastrakai on 7/1/2024.
//

#include "ColorUtils.hpp"

ImColor ColorUtils::Rainbow(float seconds, float saturation, float brightness, int index)
{
    float hue = ((NOW + index) % (int)(seconds * 1000)) / (float)(seconds * 1000);
    float r, g, b = 0;
    return ImColor::HSV(hue, saturation, brightness);
}
