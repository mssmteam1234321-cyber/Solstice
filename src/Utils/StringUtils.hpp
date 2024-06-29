#pragma once
#include <vector>
#include <string>
#include <string_view>
#include <cctype>
//
// Created by vastrakai on 6/28/2024.
//


class StringUtils {
public:
    static bool startsWith(std::string_view str, std::string_view prefix);
    static bool endsWith(std::string_view str, std::string_view suffix);
    static std::string_view trim(std::string_view str);
    static std::vector<std::string> split(std::string_view str, char delimiter);
};