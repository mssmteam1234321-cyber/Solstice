#pragma once
#include <SDK/SigManager.hpp>
//
// Created by vastrakai on 6/24/2024.
//


class MainView {
public:
    class BedrockPlatformUWP* getBedrockPlatform();
    static MainView* getInstance();
};