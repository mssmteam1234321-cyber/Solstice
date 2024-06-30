#pragma once
//
// Created by vastrakai on 6/29/2024.
//

struct ImVec4;

class ImRenderUtils {
public:
    static void addBlur(const ImVec4& pos, float strength, float radius = 0.f);
};
