//
// Created by vastrakai on 6/29/2024.
//

#include "FileUtils.hpp"

std::string FileUtils::getRoamingStatePath()
{
    // Get the appdata environment variable
    char* appdata = getenv("APPDATA");
    if (appdata == nullptr)
        return "";
    return std::string(appdata);
}
