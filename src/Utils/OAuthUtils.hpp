#pragma once
//
// Created by vastrakai on 9/3/2024.
//


class OAuthUtils {
public:
    static inline std::string sEndpoint = "https://dllserver.solstice.works/";

    static bool hasValidToken();
    static std::string getToken();
};