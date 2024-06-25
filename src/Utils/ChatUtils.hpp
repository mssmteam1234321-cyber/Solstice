#pragma once
#include <string>
//
// Created by vastrakai on 6/25/2024.
//


class ChatUtils {
public:
    static void displayClientMessage(const std::string& msg);
    static void displayClientMessage(const std::string& subcaption, const std::string& msg);
};