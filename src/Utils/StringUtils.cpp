//
// Created by vastrakai on 6/28/2024.
//

#include "StringUtils.hpp"

#include <algorithm>

bool StringUtils::startsWith(std::string_view str, std::string_view prefix)
{
    return str.substr(0, prefix.size()) == prefix;
}

bool StringUtils::endsWith(std::string_view str, std::string_view suffix)
{
    return str.substr(str.size() - suffix.size(), suffix.size()) == suffix;
}

std::string_view StringUtils::trim(std::string_view str)
{
    size_t start = 0;
    size_t end = str.size();

    while (start < end && std::isspace(str[start])) {
        start++;
    }

    while (end > start && std::isspace(str[end - 1])) {
        end--;
    }

    return str.substr(start, end - start);
}

std::vector<std::string> StringUtils::split(std::string_view str, char delimiter)
{
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = 0;

    while ((end = str.find(delimiter, start)) != std::string::npos)
    {
        result.emplace_back(str.substr(start, end - start));
        start = end + 1;
    }

    result.emplace_back(str.substr(start));

    return result;
}

// toLower and toUpper
std::string StringUtils::toLower(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
    return str;
}

std::string StringUtils::toUpper(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::toupper(c); });
    return str;
}

bool StringUtils::equalsIgnoreCase(const std::string& str1, const std::string& str2)
{
    return toLower(str1) == toLower(str2);
}
