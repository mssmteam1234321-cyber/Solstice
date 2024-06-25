#pragma once
//
// Created by vastrakai on 6/24/2024.
//


class MinecraftGame {
public:
    static MinecraftGame* getInstance();
    class ClientInstance* getPrimaryClientInstance();
};