#pragma once
//
// Created by vastrakai on 7/12/2024.
//
#include <nlohmann/json.hpp>

struct Preferences
{
    std::string mDefaultConfigName;
    std::vector<std::string> mFriends;
};

class PreferenceManager {
public:
    static std::shared_ptr<Preferences> load();
    static void save(const std::shared_ptr<Preferences>& prefs);
};