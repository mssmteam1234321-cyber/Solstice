//
// Created by vastrakai on 6/28/2024.
//

#include "StringUtils.hpp"

#include <algorithm>

#include <windows.h>

#include "spdlog/spdlog.h"


std::string StringUtils::generateUUID()
{
    // Generate a UUID v4
    std::string uuid = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";

    for (size_t i = 0; i < uuid.size(); i++)
    {
        char c = uuid[i];
        if (c == 'x')
        {
            uuid[i] = "0123456789abcdef"[std::rand() % 16];
        }
        else if (c == 'y')
        {
            uuid[i] = "89ab"[std::rand() % 4];
        }
    }

    return uuid;
}

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

bool StringUtils::containsIgnoreCase(const std::string& str, const std::string& subStr)
{
    return toLower(str).find(toLower(subStr)) != std::string::npos;
}

bool StringUtils::containsAnyIgnoreCase(const std::string& str, const std::vector<std::string>& strVector)
{
    if (std::ranges::any_of(strVector, [&str](const std::string& subStr) { return containsIgnoreCase(str, subStr); }))
    {
        return true;
    }

    return false;
}

std::string StringUtils::getClipboardText()
{
    // Try opening the clipboard
    if (! OpenClipboard(nullptr))
    {
        spdlog::error("Failed to open clipboard: {}", GetLastError());
        return "";
    }

    // Get handle of clipboard object for ANSI text
    HANDLE hData = GetClipboardData(CF_TEXT);
    if (hData == nullptr)
    {
        spdlog::error("Failed to get clipboard text: {}", GetLastError());
        return "";
    }

    // Lock the handle to get the actual text pointer
    char * pszText = static_cast<char*>( GlobalLock(hData) );
    if (pszText == nullptr)
    {
        spdlog::error("Failed to get clipboard text: {}", GetLastError());
        return "";
    }

    // Save text in a string class instance
    std::string text( pszText );

    // Release the lock
    GlobalUnlock( hData );

    // Release the clipboard
    CloseClipboard();

    return text;
}
