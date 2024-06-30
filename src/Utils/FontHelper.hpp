#pragma once
//
// Created by vastrakai on 6/29/2024.
//

#include <map>
#include <string>
#include <imgui.h>

class FontHelper {
public:
    static inline std::map<std::string, ImFont*> Fonts;

    static void load();
};