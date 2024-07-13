#pragma once
//
// Created by vastrakai on 6/29/2024.
//


class FontHelper {
public:
    static inline std::map<std::string, ImFont*> Fonts;

    static void load();
};